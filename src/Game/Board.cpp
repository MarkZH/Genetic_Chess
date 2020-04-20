#include <iostream>
#include <fstream>
#include <cctype>
#include <cassert>
#include <stdexcept>
#include <array>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <atomic>
#include <string>
#include <chrono>
using namespace std::chrono_literals;
#include <iomanip>
#include <optional>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"
#include "Moves/Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Direction.h"

#include "Players/Player.h"

#include "Utility/Exceptions.h"
#include "Utility/Random.h"
#include "Utility/String.h"
#include "Utility/Fixed_Capacity_Vector.h"

namespace
{
    const auto square_hash_values =
    []()
    {
        // One entry for each piece on each square (including no piece)
        std::array<std::array<uint64_t, 33>, 64> hash_cache;
        for(auto& square_indexed_row : hash_cache)
        {
            std::generate(square_indexed_row.begin(),
                          square_indexed_row.end(),
                          Random::random_unsigned_int64);
        }
        return hash_cache;
    }();

    const auto en_passant_hash_values =
    []()
    {
        // One entry for each file
        std::array<uint64_t, 8> en_passant_hash_cache;
        std::generate(en_passant_hash_cache.begin(),
                      en_passant_hash_cache.end(),
                      Random::random_unsigned_int64);
        return en_passant_hash_cache;
    }();

    const auto castling_hash_values =
    []()
    {
        // One entry for each rook
        std::array<uint64_t, 4> castling_hash_cache;
        std::generate(castling_hash_cache.begin(),
                      castling_hash_cache.end(),
                      Random::random_unsigned_int64);
        return castling_hash_cache;
    }();

    std::string extract_standard_fen(const std::string& original_fen)
    {
        if( ! String::contains(original_fen, "*"))
        {
            return original_fen;
        }

        auto tokens = String::split(original_fen);
        auto board_rows = String::split(tokens.front(), "/");
        if(board_rows.size() != 10)
        {
            throw std::runtime_error("Wrong number of rows in board field: " + original_fen);
        }

        auto standard_board = String::join(std::next(board_rows.begin()), std::prev(board_rows.end()), "/");
        return standard_board + " " + String::join(std::next(tokens.begin()), tokens.end(), " ");
    }

    const uint64_t switch_turn_board_hash = Random::random_unsigned_int64();

    const std::string standard_starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::atomic<Thinking_Output_Type> thinking_indicator = Thinking_Output_Type::NO_THINKING;
    std::atomic_bool move_immediately = false;
}

Board::Board(Board_Type type) noexcept : Board(standard_starting_fen)
{
    if(type == Board_Type::MUSKETEER)
    {
        board_type = type;
        pick_and_place_random_gated_pieces();
        set_unmoved_gate_guardians();
        set_initial_fen(fen());
    }
}

Board::Board(const std::string& original_fen) :
    starting_fen(String::remove_extra_whitespace(original_fen)),
    board_type(String::contains(original_fen, '*') ? Board_Type::MUSKETEER : Board_Type::STANDARD)
{
    auto input_fen = extract_standard_fen(original_fen);

    auto fen_parse = String::split(input_fen);
    if(fen_parse.size() != 6)
    {
        fen_error("Wrong number of fields (should be 6)");
    }

    auto board_parse = String::split(fen_parse.at(0), "/");
    if(board_parse.size() != 8)
    {
        fen_error("Board has wrong number of rows (should be 8)");
    }

    for(int rank = 8; rank >= 1; --rank)
    {
        char file = 'a';
        for(auto symbol : board_parse.at(size_t(8)-rank))
        {
            if(isdigit(symbol))
            {
                file += symbol - '0';
                if(file > 'h' + 1)
                {
                    fen_error("Too many squares in rank " + std::to_string(rank));
                }
            }
            else
            {
                if(file > 'h')
                {
                    fen_error("Too many squares in rank " + std::to_string(rank));
                }

                auto piece = Piece{symbol};

                if(piece.type() == Piece_Type::PAWN && (rank == 1 || rank == 8))
                {
                    fen_error("Pawns cannot be placed on the home ranks.");
                }

                if(piece.type() == Piece_Type::KING && find_king(piece.color()).is_set())
                {
                    fen_error("More than one " + color_text(piece.color()) + " king.");
                }

                place_piece(piece, {file, rank});
                ++file;
            }
        }

        if(file != 'h' + 1)
        {
            fen_error("Too few squares in rank " + std::to_string(rank));
        }
    }

    if( ! find_king(Piece_Color::WHITE).is_set())
    {
        fen_error("White king not in FEN string");
    }

    if( ! find_king(Piece_Color::BLACK).is_set())
    {
        fen_error("Black king not in FEN string");
    }

    if(fen_parse[1] == "w")
    {
        turn_color = Piece_Color::WHITE;
    }
    else if(fen_parse[1] == "b")
    {
        turn_color = Piece_Color::BLACK;
    }
    else
    {
        fen_error("Invalid character for whose turn: " + fen_parse[1]);
    }

    auto non_turn_color = opposite(whose_turn());
    if( ! safe_for_king(find_king(non_turn_color), non_turn_color))
    {
        fen_error(color_text(non_turn_color) +
                       " is in check but it is " +
                       color_text(whose_turn()) + "'s turn.");
    }

    auto castling_parse = fen_parse.at(2);
    if(castling_parse != "-")
    {
        for(auto c : castling_parse)
        {
            if( ! String::contains("KQkq", c))
            {
                fen_error(std::string("Illegal character in castling section: ") + c + "(" + castling_parse + ")");
            }

            Piece_Color piece_color = std::isupper(c) ? Piece_Color::WHITE : Piece_Color::BLACK;
            auto rook_square = Square{std::toupper(c) == 'K' ? 'h' : 'a', std::isupper(c) ? 1 : 8};
            auto king_square = Square{'e', rook_square.rank()};
            std::string side = std::toupper(c) == 'K' ? "king" : "queen";

            if(piece_on_square(rook_square) != Piece{piece_color, Piece_Type::ROOK})
            {
                fen_error("There must be a " + String::lowercase(color_text(piece_color)) + " rook on " + rook_square.string() + " to castle " + side + "side.");
            }
            set_unmoved(rook_square);

            if(piece_on_square(king_square) != Piece{piece_color, Piece_Type::KING})
            {
                fen_error("There must be a " + String::lowercase(color_text(piece_color)) + " king on " + king_square.string() + " to castle.");
            }
            set_unmoved(king_square);
        }
    }

    auto en_passant_parse = fen_parse.at(3);
    if(en_passant_parse != "-")
    {
        if(en_passant_parse.size() != 2)
        {
            fen_error("Invalid en passant square.");
        }

        make_en_passant_targetable({en_passant_parse[0], en_passant_parse[1] - '0'});

        if( ! en_passant_target.is_set())
        {
            fen_error("Invalid en passant square.");
        }

        if(piece_on_square(en_passant_target))
        {
            fen_error("Piece is not allowed on en passant target square.");
        }

        auto last_move_pawn = piece_on_square(en_passant_target + Square_Difference{0, whose_turn() == Piece_Color::WHITE ? -1 : 1});;
        if(last_move_pawn != Piece{opposite(whose_turn()), Piece_Type::PAWN})
        {
            fen_error("There must be a pawn past the en passant target square.");
        }
    }

    // Fill repeat counter to indicate moves since last
    // pawn move or capture.
    auto fifty_move_count_input = String::to_number<size_t>(fen_parse.at(4));
    if(fifty_move_count_input > 100)
    {
        fen_error("Halfmove clock value too large.");
    }
    add_board_position_to_repeat_record();
    while(moves_since_pawn_or_capture() < fifty_move_count_input)
    {
        add_to_repeat_count(Random::random_unsigned_int64());
    }

    first_full_move_label = String::to_number<size_t>(fen_parse.at(5));

    // Musketeer Board gates
    if(String::contains(original_fen, '*'))
    {
        auto tokens = String::split(original_fen);
        auto board_rows = String::split(tokens.front(), "/");
        for(auto gate_index : {0, 9})
        {
            auto gate = board_rows[gate_index];
            auto gate_color = gate_index == 0 ? Piece_Color::BLACK : Piece_Color::WHITE;
            for(size_t i = 0; i < gate.size(); ++i)
            {
                if(gate[i] != '*')
                {
                    auto gated_piece = Piece{gate[i]};
                    gated_pieces[static_cast<int>(gate_color)][i] = gated_piece;
                    if(std::count(gated_piece_types.begin(), gated_piece_types.end(), gated_piece.type()) == 0)
                    {
                        gated_piece_types.push_back(gated_piece.type());
                    }
                }
            }
        }

        if(gated_piece_types.size() != 2)
        {
            fen_error("Could not identify the two gated pieces");
        }

        set_unmoved_gate_guardians();
    }

    if(fen() != starting_fen)
    {
        fen_error("Result: " + fen());
    }

    recreate_move_caches();

    // In case a listed en passant target is not actually a legal move.
    repeat_count.pop_back();
    add_board_position_to_repeat_record();
}

Board::Board(Piece_Type first_gated_piece, Piece_Type second_gated_piece) : Board(standard_starting_fen)
{
    auto valid_gated_piece = [](auto piece_type) { return static_cast<int>(piece_type) > static_cast<int>(Piece_Type::KING); };
    if( ! valid_gated_piece(first_gated_piece) || 
        ! valid_gated_piece(second_gated_piece) ||
       first_gated_piece == second_gated_piece)
    {
        throw std::invalid_argument("Gated pieces must be of fairy type and different. Invalid choices: " +
                                    Piece{Piece_Color::WHITE, first_gated_piece}.pgn_symbol() +
                                    ", " +
                                    Piece{Piece_Color::WHITE, second_gated_piece}.pgn_symbol());
    }

    board_type = Board_Type::MUSKETEER;
    randomly_place_gated_pieces(first_gated_piece, second_gated_piece);
    set_unmoved_gate_guardians();
    set_initial_fen(fen());
}

void Board::fen_error(const std::string& reason) const
{
    throw std::invalid_argument("Bad FEN input: " + starting_fen + "\n" + reason);
}

Piece& Board::piece_on_square(Square square) noexcept
{
    return board[square.index()];
}

Piece Board::piece_on_square(Square square) const noexcept
{
    return board[square.index()];
}

void Board::set_unmoved(Square square) noexcept
{
    update_board_hash(square); // remove reference to moved piece
    unmoved_positions[square.index()] = true;
    update_board_hash(square);
}

bool Board::is_in_legal_moves_list(const Move&) const noexcept
{
    return true;
    //return std::find(legal_moves().begin(), legal_moves().end(), &move) != legal_moves().end();
}

std::string Board::fen() const noexcept
{
    std::string s;

    if(board_type == Board_Type::MUSKETEER)
    {
        s += gate_fen(Piece_Color::BLACK) + '/';
    }

    for(int rank = 8; rank >= 1; --rank)
    {
        int empty_count = 0;
        if(rank < 8)
        {
            s.push_back('/');
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            auto piece = piece_on_square({file, rank});
            if( ! piece)
            {
                ++empty_count;
            }
            else
            {
                if(empty_count > 0)
                {
                    s += std::to_string(empty_count);
                    empty_count = 0;
                }
                s.push_back(piece.fen_symbol());
            }
        }

        if(empty_count > 0)
        {
            s += std::to_string(empty_count);
        }
    }

    if(board_type == Board_Type::MUSKETEER)
    {
        s += '/' + gate_fen(Piece_Color::WHITE);
    }

    s.push_back(' ');
    s.push_back(whose_turn() == Piece_Color::WHITE ? 'w' : 'b');
    s.push_back(' ');

    for(int base_rank : {1, 8})
    {
        if( ! piece_has_moved({'e', base_rank})) // has king moved?
        {
            for(char rook_file : {'h', 'a'})
            {
                if( ! piece_has_moved({rook_file, base_rank})) // has rook moved?
                {
                    char mark = (rook_file == 'h' ? 'K' : 'Q');
                    if(base_rank == 8)
                    {
                        mark = std::tolower(mark);
                    }
                    s.push_back(mark);
                }
            }
        }
    }

    if(s.back() == ' ')
    {
        s.push_back('-');
    }
    s.push_back(' ');

    if(en_passant_target.is_set())
    {
        s += en_passant_target.string();
    }
    else
    {
        s.push_back('-');
    }

    return s + " " +
        std::to_string(moves_since_pawn_or_capture()) + " " +
        std::to_string(1 + ply_count()/2);
}

std::string Board::gate_fen(Piece_Color color) const noexcept
{
    std::string result;
    for(auto piece : gated_pieces[static_cast<int>(color)])
    {
        result.push_back(piece ? piece.fen_symbol() : '*');
    }
    return result;
}

std::string Board::original_fen() const noexcept
{
    return starting_fen;
}

Game_Result Board::submit_move(const Move& move) noexcept
{
    update_board(move);
    return move_result();
}

Game_Result Board::submit_move(const std::string& move)
{
    return submit_move(create_move(move));
}

size_t Board::ply_count() const noexcept
{
    auto first_move = game_length()%2 == 0 ? whose_turn() : opposite(whose_turn());
    return 2*(first_full_move_label - 1) + (first_move == Piece_Color::WHITE ? 0 : 1) + game_length();
}
std::vector<const Move*> Board::derive_moves(const std::string& new_fen) const noexcept
{
    std::string goal_fen;
    size_t moves_to_derive_count;
    try
    {
        auto new_board = Board(new_fen);
        goal_fen = new_board.fen();
        moves_to_derive_count = new_board.ply_count() - ply_count();
    }
    catch(const std::invalid_argument&)
    {
        return {};
    }

    if(moves_to_derive_count > 2 || moves_to_derive_count < 1)
    {
        return {};
    }

    for(auto first_move : legal_moves())
    {
        auto first_move_board = *this;
        first_move_board.submit_move(*first_move);
        if(moves_to_derive_count == 1)
        {
            if(first_move_board.fen() == goal_fen)
            {
                return {first_move};
            }
        }
        else
        {
            for(auto second_move : first_move_board.legal_moves())
            {
                auto second_move_board = first_move_board;
                second_move_board.submit_move(*second_move);
                if(second_move_board.fen() == goal_fen)
                {
                    return {first_move, second_move};
                }
            }
        }
    }

    return {};
}

Game_Result Board::move_result() const noexcept
{
    if(no_legal_moves())
    {
        if(king_is_in_check())
        {
            return Game_Result(opposite(whose_turn()), Game_Result_Type::CHECKMATE);
        }
        else
        {
            return Game_Result(Winner_Color::NONE, Game_Result_Type::STALEMATE);
        }
    }

    // An insufficient material draw can only happen after a capture
    // or a pawn promotion to a minor piece, both of which clear the
    // repeat_count tracker.
    if(moves_since_pawn_or_capture() == 0 && ! enough_material_to_checkmate())
    {
        return Game_Result(Winner_Color::NONE, Game_Result_Type::INSUFFICIENT_MATERIAL);
    }

    if(current_board_position_repeat_count() >= 3)
    {
        return Game_Result(Winner_Color::NONE, Game_Result_Type::THREEFOLD_REPETITION);
    }

    // "Move" means both players move, so the fifty-move rule is
    // triggered after 100 player moves
    if(moves_since_pawn_or_capture() >= 100)
    {
        return Game_Result(Winner_Color::NONE, Game_Result_Type::FIFTY_MOVE);
    }

    return {};
}

void Board::update_board(const Move& move) noexcept
{
    assert(is_in_legal_moves_list(move));

    ++game_move_count;
    previous_move = &move;
    move_piece(move);
    move.side_effects(*this);

    turn_color = opposite(turn_color);
    update_whose_turn_hash();

    recreate_move_caches();

    add_board_position_to_repeat_record();
}

size_t Board::game_length() const noexcept
{
    return game_move_count;
}

const Move* Board::last_move() const noexcept
{
    return previous_move;
}

int Board::castling_direction(Piece_Color player) const noexcept
{
    return castling_movement[static_cast<int>(player)];
}

const Move& Board::create_move(std::string move_text) const
{
    const static auto optional_end_marks = "+#?!";
    move_text = move_text.substr(0, move_text.find_first_of(optional_end_marks));
    auto san_move_iter = std::find_if(legal_moves().begin(), legal_moves().end(),
                                      [this, &move_text](auto move)
                                      {
                                          auto legal_move_text = move->algebraic(*this);
                                          legal_move_text = legal_move_text.substr(0, legal_move_text.find_first_of(optional_end_marks));
                                          return legal_move_text == move_text;
                                      });
    if(san_move_iter != legal_moves().end())
    {
        return **san_move_iter;
    }

    auto coordinate_move_iter = std::find_if(legal_moves().begin(), legal_moves().end(),
                                             [&move_text](auto move)
                                             {
                                                 return move->coordinates() == move_text;
                                             });
    if(coordinate_move_iter != legal_moves().end())
    {
        return **coordinate_move_iter;
    }
    else
    {
        throw Illegal_Move("The move text is not a valid or legal move: " + move_text);
    }
}

void Board::move_piece(const Move& move) noexcept
{
    auto piece_first_move = ! piece_has_moved(move.start());
    auto moving_king_from_check = king_is_in_check() && piece_on_square(move.start()).type() == Piece_Type::KING;

    if(piece_on_square(move.end()))
    {
        remove_piece(move.end());
        clear_repeat_count();
    }

    auto moving_piece = piece_on_square(move.start());
    if(moving_piece.type() == Piece_Type::PAWN)
    {
        clear_repeat_count();
    }
    remove_piece(move.start());
    place_piece(moving_piece, move.end());

    if(piece_first_move && ! moving_king_from_check)
    {
        other_move_effects(move);
    }

    clear_en_passant_target();
}

Piece_Color Board::whose_turn() const noexcept
{
    return turn_color;
}

const std::vector<const Move*>& Board::legal_moves() const noexcept
{
    return legal_moves_cache;
}

void Board::remove_piece(Square square) noexcept
{
    place_piece({}, square);
}

void Board::place_piece(Piece piece, Square square) noexcept
{
    update_board_hash(square); // XOR out piece on square

    auto old_piece = piece_on_square(square);
    piece_on_square(square) = piece;

    auto update_rook_hashes = old_piece && old_piece.type() == Piece_Type::KING && unmoved_positions[square.index()];
    if(update_rook_hashes)
    {
        // XOR out castling rights on rook squares
        update_board_hash({'a', square.rank()});
        update_board_hash({'h', square.rank()});
    }

    unmoved_positions[square.index()] = false;

    if(update_rook_hashes)
    {
        // XOR in hashes with no castling rights
        update_board_hash({'a', square.rank()});
        update_board_hash({'h', square.rank()});
    }

    update_board_hash(square); // XOR in new piece on square

    if(piece && piece.type() == Piece_Type::KING)
    {
        king_location[static_cast<int>(piece.color())] = square;
    }
}

void Board::recreate_attacks() noexcept
{
    blockable_attacks = {};
    unblockable_attacks = {};
    blocked_attacks = {};

    for(auto square : Square::all_squares())
    {
        auto piece = piece_on_square(square);
        if( ! piece)
        {
            continue;
        }

        auto attacking_color = piece.color();
        auto vulnerable_king = Piece{opposite(attacking_color), Piece_Type::KING};
        for(const auto& attack_move_list : piece.attacking_move_lists(square))
        {
            auto blocked = false;
            for(auto attack : attack_move_list)
            {
                auto attacked_square = attack->end();
                auto attacked_index = attacked_square.index();
                if(attack->is_blockable())
                {
                    if(blocked)
                    {
                        blocked_attacks[static_cast<int>(attacking_color)][attacked_index][attack->attack_index()] = true;
                    }
                    else
                    {
                        blockable_attacks[static_cast<int>(attacking_color)][attacked_index][attack->attack_index()] = true;

                        auto blocking_piece = piece_on_square(attacked_square);
                        if(blocking_piece && blocking_piece != vulnerable_king)
                        {
                            blocked = true;
                        }
                    }
                }
                else
                {
                    unblockable_attacks[static_cast<int>(attacking_color)][attacked_index][attack->attack_index()] = true;
                }
            }
        }
    }
}

std::bitset<24> Board::moves_attacking_square(Square square, Piece_Color attacking_color) const noexcept
{
    auto result = blockable_attacks[static_cast<int>(attacking_color)][square.index()];
    for(size_t i = 0; i < result.size(); ++i)
    {
        result[i] = result[i] || unblockable_attacks[static_cast<int>(attacking_color)][square.index()][i];
    }
    return result;
}

std::bitset<24> Board::checking_moves() const noexcept
{
    return moves_attacking_square(find_king(whose_turn()), opposite(whose_turn()));
}

bool Board::king_is_in_check() const noexcept
{
    return checking_moves().any();
}

bool Board::safe_for_king(Square square, Piece_Color king_color) const noexcept
{
    return moves_attacking_square(square, opposite(king_color)).none();
}

bool Board::blocked_attack(Square square, Piece_Color attacking_color) const noexcept
{
    return blocked_attacks[static_cast<int>(attacking_color)][square.index()].any();
}

bool Board::king_is_in_check_after_move(const Move& move) const noexcept
{
    auto king_square = find_king(whose_turn());
    if(move.start() == king_square)
    {
        return ! safe_for_king(move.end(), whose_turn());
    }

    if(king_is_in_check())
    {
        if(king_multiply_checked())
        {
            return true;
        }

        assert(checking_square.is_set());
        if(move.end() == checking_square || (check_is_blockable() && in_line_in_order(checking_square, move.end(), king_square)))
        {
            return piece_is_pinned(move.start());
        }

        if(move.is_en_passant())
        {
            auto captured_pawn_square = Square{move.end().file(), move.start().rank()};
            return checking_square != captured_pawn_square || piece_is_pinned(move.start());
        }

        // Nothing is done about the check
        return true;
    }

    if(piece_is_pinned(move.start()))
    {
        return ! moves_are_parallel(move.movement(), king_square - move.start()) ||
               ! all_empty_between(move.start(), move.end());
    }

    if(move.is_en_passant())
    {
        if(king_square.rank() != move.start().rank())
        {
            return false;
        }

        auto squares = Square::square_line_from(king_square, (move.start() - king_square).step());
        auto revealed_attacker = std::find_if(squares.begin(), squares.end(),
                                              [this](auto square)
                                              {
                                                  auto piece = piece_on_square(square);
                                                  return piece && piece.color() == opposite(whose_turn()) &&
                                                      (piece.type() == Piece_Type::QUEEN || piece.type() == Piece_Type::ROOK);
                                              });

        return revealed_attacker != squares.end() &&
               in_line_in_order(king_square, move.start(), *revealed_attacker) &&
               std::count_if(squares.begin(), revealed_attacker, [this](auto square) { return piece_on_square(square); }) == 2;
    }

    return false;
}

bool Board::no_legal_moves() const noexcept
{
    return legal_moves().empty();
}

template<typename OutputStream, typename DataType>
void output_game_header_line(OutputStream& output, const std::string& heading, const DataType& data)
{
    output << "[" << heading << " \"" << data << "\"]\n";
}

template<typename OutputStream, typename DataType>
void print_game_header_line(OutputStream& output, const std::string& heading, const DataType& data)
{
    output_game_header_line(output, heading, data);
}

template<typename OutputStream>
void print_game_header_line(OutputStream& output, const std::string& heading, const std::string& data)
{
    output_game_header_line(output, heading, data.empty() ? "?" : data);
}

template<typename OutputStream>
void print_game_header_line(OutputStream& output, const std::string& heading, const char* data)
{
    print_game_header_line(output, heading, std::string{data});
}

void Board::print_game_record(const std::vector<const Move*>& game_record_listing,
                              const Player* white,
                              const Player* black,
                              const std::string& file_name,
                              const Game_Result& result,
                              const Clock& game_clock,
                              const std::string& event_name,
                              const std::string& location) const noexcept
{
    static std::mutex write_lock;
    auto write_lock_guard = std::lock_guard(write_lock);

    static int game_number = 0;
    static std::string last_used_file_name;
    if(game_number == 0 || file_name != last_used_file_name)
    {
        game_number = 1;
        last_used_file_name = file_name;
        std::ifstream ifs(file_name);
        std::string line;
        while(std::getline(ifs, line))
        {
            if(String::starts_with(line, "[Round"))
            {
                auto round_number = String::to_number<int>(String::split(line, "\"").at(1));
                if(round_number >= game_number)
                {
                    game_number = round_number + 1;
                }
            }
        }
    }

    std::ofstream ofs(file_name, std::ios::app);
    std::ostream& out_stream = (ofs ? ofs : std::cout);

    print_game_header_line(out_stream, "Event", event_name);
    print_game_header_line(out_stream, "Site", location);
    print_game_header_line(out_stream, "Date", String::date_and_time_format(game_clock.game_start_date_and_time(), "%Y.%m.%d"));
    print_game_header_line(out_stream, "Round", game_number++);

    for(auto player_color : {Piece_Color::WHITE, Piece_Color::BLACK})
    {
        auto player = player_color == Piece_Color::WHITE ? white : black;
        auto player_name = player ? player->name() : std::string{};
        if(player_name.empty())
        {
            auto other_player = player_color == Piece_Color::WHITE ? black : white;
            player_name = other_player ? other_player->opponent_name() : std::string{};
        }
        print_game_header_line(out_stream, color_text(player_color), player_name);
    }

    // Get actual result
    auto last_move_result = move_result();
    auto actual_result = last_move_result.game_has_ended() ? last_move_result : result;

    print_game_header_line(out_stream, "Result", actual_result.game_ending_annotation());

    print_game_header_line(out_stream, "Time", String::date_and_time_format(game_clock.game_start_date_and_time(), "%H:%M:%S"));

    if(game_clock.is_in_use())
    {
        std::ostringstream time_control_spec;
        if(game_clock.moves_per_time_period() > 0)
        {
            time_control_spec << game_clock.moves_per_time_period() << '/';
        }
        time_control_spec << game_clock.initial_time().count();
        if(game_clock.increment(Piece_Color::WHITE) > 0.0s)
        {
            time_control_spec << '+' << game_clock.increment(Piece_Color::WHITE).count();
        }
        print_game_header_line(out_stream, "TimeControl", time_control_spec.str());
        print_game_header_line(out_stream, "TimeLeftWhite", game_clock.time_left(Piece_Color::WHITE).count());
        print_game_header_line(out_stream, "TimeLeftBlack", game_clock.time_left(Piece_Color::BLACK).count());
    }

    if( ! actual_result.ending_reason().empty() && ! String::contains(actual_result.ending_reason(), "mates"))
    {
        print_game_header_line(out_stream, "Termination", actual_result.ending_reason());
    }

    if(starting_fen != standard_starting_fen)
    {
        print_game_header_line(out_stream, "SetUp", 1);
        print_game_header_line(out_stream, "FEN", starting_fen);
    }

    auto commentary_board = Board(original_fen());
    auto starting_turn_offset = size_t(commentary_board.whose_turn() == Piece_Color::WHITE ? 0 : 1);

    for(size_t i = 0; i < game_record_listing.size(); ++i)
    {
        auto step = first_full_move_label + (i + starting_turn_offset)/2;
        if(commentary_board.whose_turn() == Piece_Color::WHITE || i == 0)
        {
            out_stream << '\n' << step << ".";
            if(i == 0 && commentary_board.whose_turn() == Piece_Color::BLACK)
            {
                out_stream << " ...";
            }
        }

        auto next_move = game_record_listing.at(i);
        out_stream << " " << next_move->algebraic(commentary_board);
        auto current_player = (commentary_board.whose_turn() == Piece_Color::WHITE ? white : black);
        if(current_player)
        {
            auto commentary = String::trim_outer_whitespace(current_player->commentary_for_next_move(commentary_board, step));
            if( ! commentary.empty())
            {
                out_stream << " " << commentary;
            }
        }
        commentary_board.submit_move(*next_move);
    }
    out_stream << " " << actual_result.game_ending_annotation() << "\n\n\n";

    assert(commentary_board.fen() == fen());
}

void Board::make_en_passant_targetable(Square square) noexcept
{
    if(en_passant_target.is_set())
    {
        update_board_hash(en_passant_target); // XOR out previous en passant target
    }

    en_passant_target = square;

    if(square.is_set())
    {
        update_board_hash(square);
    }
}

bool Board::is_en_passant_targetable(Square square) const noexcept
{
    return en_passant_target == square;
}

void Board::clear_en_passant_target() noexcept
{
    make_en_passant_targetable({});
}

bool Board::piece_has_moved(Square square) const noexcept
{
    return ! unmoved_positions[square.index()];
}

Square Board::find_king(Piece_Color color) const noexcept
{
    return king_location[static_cast<int>(color)];
}

void Board::recreate_move_caches() noexcept
{
    recreate_attacks();
    last_pin_check_square = Square{};
    checking_square = king_is_in_check() ? find_checking_square() : Square{};
    prior_moves_count = legal_moves_cache.size();
    legal_moves_cache.clear();
    for(auto square : Square::all_squares())
    {
        auto piece = piece_on_square(square);
        if(piece && piece.color() == whose_turn())
        {
            for(const auto& move_list : piece.move_lists(square))
            {
                for(auto move : move_list)
                {
                    if(move->is_legal(*this))
                    {
                        legal_moves_cache.push_back(move);
                    }

                    if(piece_on_square(move->end()))
                    {
                        break;
                    }
                }
            }
        }
    }

    add_other_moves();

    if(std::none_of(legal_moves_cache.begin(),
                    legal_moves_cache.end(),
                    [](auto move) { return move->is_en_passant(); }))
    {
        clear_en_passant_target();
    }

    material_changing_move_available = std::any_of(legal_moves_cache.begin(), legal_moves_cache.end(),
                                                   [this](auto move)
                                                   {
                                                       return move_changes_material(*move);
                                                   });
}

void Board::add_other_moves() noexcept
{
    if(board_type != Board_Type::MUSKETEER)
    {
        return;
    }

    gated_pawn_promotions.clear();
    auto pawn = Piece{whose_turn(), Piece_Type::PAWN};
    auto pre_promotion_rank = whose_turn() == Piece_Color::WHITE ? 7 : 2;
    for(auto file = 'a'; file <= 'h'; ++file)
    {
        auto square = Square{file, pre_promotion_rank};
        if(piece_on_square(square) == pawn)
        {
            for(auto gated_piece_type : gated_piece_types)
            {
                for(auto file_change : {-1, 0, 1})
                {
                    std::optional<Pawn_Promotion> move;
                    if(file_change == 0)
                    {
                        move = Pawn_Promotion(gated_piece_type, pawn.color(), square.file());
                    }
                    else
                    {
                        if((file_change == 1 && file == 'h') || (file_change == -1 && file == 'a'))
                        {
                            continue;
                        }
                        move = Pawn_Promotion(gated_piece_type, pawn.color(), file, file_change == 1 ? Direction::RIGHT : Direction::LEFT);
                    }

                    if(move->is_legal(*this))
                    {
                        gated_pawn_promotions.push_back(*move);
                    }
                }
            }
        }
    }

    for(const auto& move : gated_pawn_promotions)
    {
        legal_moves_cache.push_back(&move);
    }
}

Square Board::find_checking_square() const noexcept
{
    auto checks = checking_moves();
    size_t checking_index = 0;
    while( ! checks[checking_index])
    {
        ++checking_index;
    }
    auto step = Move::attack_direction_from_index(checking_index);
    const auto& king_square = find_king(whose_turn());
    auto squares = Square::square_line_from(king_square, -step);
    return *std::find_if(squares.begin(), squares.end(),
                         [this, &king_square](auto square)
                         {
                             auto attacker = piece_on_square(square);
                             if( ! attacker)
                             {
                                 return false;
                             }

                             if(check_is_blockable())
                             {
                                 return true;
                             }

                             if(attacker.color() != opposite(whose_turn()))
                             {
                                 return false;
                             }

                             for(auto attack_list : attacker.attacking_move_lists(square))
                             {
                                 for(auto attack : attack_list)
                                 {
                                     if( ! attack->is_blockable() && attack->end() == king_square)
                                     {
                                         return true;
                                     }
                                 }
                             }

                             return false;
                         });
}

bool Board::enough_material_to_checkmate(Piece_Color piece_color) const noexcept
{
    auto piece_is_right = [piece_color](auto piece, auto type) { return piece == Piece{piece_color, type}; };

    if(std::any_of(board.begin(), board.end(),
                   [piece_is_right, piece_color](auto piece)
                   {
                       return piece_is_right(piece, Piece_Type::QUEEN) ||
                              piece_is_right(piece, Piece_Type::ROOK) ||
                              piece_is_right(piece, Piece_Type::PAWN) ||
                              (piece && piece.color() == piece_color &&
                               static_cast<int>(piece.type()) > static_cast<int>(Piece_Type::KING));
                   }))
    {
        return true;
    }

    auto knight_count = std::count_if(board.begin(), board.end(), [piece_is_right](auto piece) { return piece_is_right(piece, Piece_Type::KNIGHT); });
    if(knight_count > 1)
    {
        return true;
    }

    auto bishop_on_square_color =
        [this, piece_is_right](Square_Color square_color, Square square)
        {
            return piece_is_right(piece_on_square(square), Piece_Type::BISHOP) && square.color() == square_color;
        };

    auto squares = Square::all_squares();
    auto bishops_on_white = std::any_of(squares.begin(), squares.end(),
                                        [bishop_on_square_color](auto square) { return bishop_on_square_color(Square_Color::WHITE, square); });
    auto bishops_on_black = std::any_of(squares.begin(), squares.end(),
                                        [bishop_on_square_color](auto square) { return bishop_on_square_color(Square_Color::BLACK, square); });
    return (bishops_on_white && bishops_on_black) || (knight_count > 0 && (bishops_on_white || bishops_on_black));
}

bool Board::enough_material_to_checkmate() const noexcept
{
    auto piece_is_right = [](auto piece, auto type) { return piece && piece.type() == type; };

    if(std::any_of(board.begin(), board.end(),
                   [piece_is_right](auto piece)
                   {
                       return piece_is_right(piece, Piece_Type::QUEEN) ||
                              piece_is_right(piece, Piece_Type::ROOK) ||
                              piece_is_right(piece, Piece_Type::PAWN) ||
                              (piece && static_cast<int>(piece.type()) > static_cast<int>(Piece_Type::KING));
                   }))
    {
        return true;
    }

    auto knight_count = std::count_if(board.begin(), board.end(), [piece_is_right](auto piece) { return piece_is_right(piece, Piece_Type::KNIGHT); });
    if(knight_count > 1)
    {
        return true;
    }

    auto bishop_on_square_color =
        [this, piece_is_right](Square_Color square_color, Square square)
        {
            return piece_is_right(piece_on_square(square), Piece_Type::BISHOP) && square.color() == square_color;
        };

    auto squares = Square::all_squares();
    auto bishops_on_white = std::any_of(squares.begin(), squares.end(),
                                        [bishop_on_square_color](auto square) { return bishop_on_square_color(Square_Color::WHITE, square); });
    auto bishops_on_black = std::any_of(squares.begin(), squares.end(),
                                        [bishop_on_square_color](auto square) { return bishop_on_square_color(Square_Color::BLACK, square); });
    return (bishops_on_white && bishops_on_black) || (knight_count > 0 && (bishops_on_white || bishops_on_black));
}

void Board::set_thinking_mode(Thinking_Output_Type mode) noexcept
{
    thinking_indicator = mode;
}

Thinking_Output_Type Board::thinking_mode() noexcept
{
    return thinking_indicator;
}

void Board::pick_move_now() noexcept
{
    move_immediately = true;
}

void Board::choose_move_at_leisure() noexcept
{
    move_immediately = false;
}

bool Board::must_pick_move_now() noexcept
{
    return move_immediately;
}

void Board::update_board_hash(Square square) noexcept
{
    current_board_hash ^= square_hash(square);
}

void Board::update_whose_turn_hash() noexcept
{
    current_board_hash ^= switch_turn_board_hash;
}

uint64_t Board::square_hash(Square square) const noexcept
{
    assert(square.inside_board());

    auto piece = piece_on_square(square);
    auto index = square.index();
    auto result = square_hash_values[index][piece.index()];
    if(piece &&
       piece.type() == Piece_Type::ROOK &&
       ! piece_has_moved(square) &&
       ! piece_has_moved(king_location[static_cast<int>(piece.color())]))
    {
        auto on_first_rank = (index%8 == 0);
        auto on_first_file = (index/8 == 0);
        result ^= castling_hash_values[2*on_first_file + on_first_rank];
    }

    if( ! piece && is_en_passant_targetable(square))
    {
        auto file_index = index/8;
        result = en_passant_hash_values[file_index];
    }

    return result;
}

uint64_t Board::board_hash() const noexcept
{
    return current_board_hash;
}

bool Board::move_captures(const Move& move) const noexcept
{
    auto attacked_piece = piece_on_square(move.end());

    // Assert move is actually legal
    assert(is_in_legal_moves_list(move));
    assert( ! attacked_piece || (move.can_capture() && attacked_piece.color() == opposite(whose_turn())));

    return attacked_piece || move.is_en_passant();
}

bool Board::move_changes_material(const Move& move) const noexcept
{
    return move_captures(move) || move.promotion_piece_symbol();
}

bool Board::material_change_possible() const noexcept
{
    return material_changing_move_available;
}

bool Board::king_multiply_checked() const noexcept
{
    auto attacker_index = static_cast<int>(opposite(whose_turn()));
    auto king_square_index = find_king(whose_turn()).index();
    auto blockable_attack_count = blockable_attacks[attacker_index][king_square_index].count();
    auto unblockable_attack_count = unblockable_attacks[attacker_index][king_square_index].count();
    return blockable_attack_count + unblockable_attack_count > 1;
}

bool Board::all_empty_between(Square start, Square end) const noexcept
{
    assert(straight_line_move(start, end));
    auto squares = Square::squares_between(start, end);
    return std::all_of(squares.begin(), squares.end(), [this](auto square) { return ! piece_on_square(square); });
}

bool Board::check_is_blockable() const noexcept
{
    return unblockable_attacks[static_cast<int>(opposite(whose_turn()))][find_king(whose_turn()).index()].none();
}

bool Board::piece_is_pinned(Square square) const noexcept
{
    if(square == last_pin_check_square)
    {
        return last_pin_result;
    }
    last_pin_check_square = square;

    const auto& king_square = find_king(whose_turn());
    if(king_square == square)
    {
        return last_pin_result = false; // king is never pinned
    }

    if( ! straight_line_move(square, king_square))
    {
        return last_pin_result = false;
    }

    auto diff = king_square - square;
    if(blockable_attacks[static_cast<int>(opposite(whose_turn()))][square.index()][Move::attack_index(diff)])
    {
        // The blockable_attacks check guarantees that there is an opposing piece attacking
        // the queried square in the same direction towards the friendly king and that attack
        // can continue farther to reach the king.
        return last_pin_result = all_empty_between(king_square, square);
    }
    else
    {
        return last_pin_result = false;
    }
}

void Board::add_board_position_to_repeat_record() noexcept
{
    add_to_repeat_count(board_hash());
}

void Board::add_to_repeat_count(uint64_t new_hash) noexcept
{
    repeat_count.push_back(new_hash);
}

ptrdiff_t Board::current_board_position_repeat_count() const noexcept
{
    return repeat_count.count(board_hash());
}

size_t Board::moves_since_pawn_or_capture() const noexcept
{
    return repeat_count.size() - 1;
}

void Board::clear_repeat_count() noexcept
{
    repeat_count.clear();
}

size_t Board::castling_move_index(Piece_Color player) const noexcept
{
    return castling_index[static_cast<int>(player)];
}

Board Board::without_random_pawn() const noexcept
{
    assert(std::any_of(board.begin(), board.end(), [](auto p) { return p && p.type() == Piece_Type::PAWN; }));

    auto result = *this;
    while(true)
    {
        auto square = Square('a' + Random::random_integer(0, 7), Random::random_integer(1, 8));
        auto piece = result.piece_on_square(square);
        if(piece && piece.type() == Piece_Type::PAWN)
        {
            result.remove_piece(square);
            return result;
        }
    }
}

std::vector<const Move*> Board::quiescent(const std::array<double, 16>& piece_values) const noexcept
{
    if( ! previous_move)
    {
        return {};
    }

    // The capture_move at index i results in the state_value at index i + 1
    std::vector<const Move*> capture_moves;
    std::vector<double> state_values = {0.0};

    const auto player_color = whose_turn();
    auto current_board = *this;
    const auto square = previous_move->end();
    while( ! current_board.safe_for_king(square, opposite(current_board.whose_turn())))
    {
        std::vector<const Move*> capturing_moves;
        std::copy_if(current_board.legal_moves().begin(), current_board.legal_moves().end(), std::back_inserter(capturing_moves),
                        [square](auto move) { return move->end() == square; });

        if(capturing_moves.empty())
        {
            // This can happen when the only attacking piece is a king
            // and the attacked piece is guarded.
            break;
        }

        // Attack with the weakest piece first
        auto move = *std::min_element(capturing_moves.begin(), capturing_moves.end(),
                                      [&piece_values, &current_board](auto move1, auto move2)
                                      {
                                          return piece_values[static_cast<int>(current_board.piece_on_square(move1->start()).type())] <
                                                 piece_values[static_cast<int>(current_board.piece_on_square(move2->start()).type())];
                                      });

        // Make sure that an exchange does not lose material
        auto moving_piece = current_board.piece_on_square(move->start());
        auto attacked_piece = current_board.piece_on_square(move->end());
        current_board.submit_move(*move);
        capture_moves.push_back(move);
        state_values.push_back(state_values.back() + (moving_piece.color() == player_color ? +1 : -1)*piece_values[static_cast<int>(attacked_piece.type())]);
    }


    if(capture_moves.empty())
    {
        return capture_moves;
    }

    // Make sure to stop before either player ends up in an
    // avoidable loss of material.
    auto minimax_index = state_values.size() - 1;
    auto minimax_value = state_values[minimax_index];
    for(auto index = state_values.size() - 2; index < state_values.size(); --index)
    {
        auto value = state_values[index];

        // Even indices indicate a player's choice (maximize score).
        // Odd indices indicate an opponent's choice (minimize score).
        if(index % 2 == 0)
        {
            if(value > minimax_value)
            {
                minimax_value = value;
                minimax_index = index;
            }
        }
        else
        {
            if(value < minimax_value)
            {
                minimax_value = value;
                minimax_index = index;
            }
        }
    }

    return {capture_moves.begin(), capture_moves.begin() + minimax_index};
}

size_t Board::previous_moves_count() const noexcept
{
    return prior_moves_count;
}

std::string Board::extra_move_mark(const Move& move) const noexcept
{
    auto next_board = *this;
    next_board.submit_move(move);
    auto gated_piece = const_cast<const Board&>(next_board).piece_on_square(move.start());
    if(gated_piece)
    {
        return "/" + gated_piece.pgn_symbol();
    }
    else
    {
        return {};
    }
}

void Board::ascii_draw() const noexcept
{
    const auto symbol_slot_width = 2;
    const auto indentation = 2;
    const auto file_label_location = 0;

    std::cout << std::endl;
    ascii_draw_above_board(indentation, symbol_slot_width);
    for(auto rank = 8; rank >= 0; --rank)
    {
        if(rank == file_label_location)
        {
            ascii_draw_below_board(indentation, symbol_slot_width);
            std::cout << std::setw(indentation) << std::right << "";
        }
        else
        {
            std::cout << std::setw(indentation) << std::left << rank << std::right;
        }

        for(auto file = 'a'; file <= 'h'; ++file)
        {
            if(rank == file_label_location)
            {
                std::cout << std::setw(symbol_slot_width) << file;
            }
            else
            {
                auto piece = piece_on_square({file, rank});
                if(piece)
                {
                    std::cout << std::setw(symbol_slot_width) << piece.fen_symbol();
                }
                else
                {
                    std::cout << std::setw(symbol_slot_width) << ".";
                }
            }
        }

        std::cout << std::endl;
    }
}

void Board::ascii_draw_above_board(int indentation, int symbol_width) const noexcept
{
    ascii_draw_gate(Piece_Color::BLACK, indentation, symbol_width);
}

void Board::ascii_draw_gate(Piece_Color color, int indentation, int symbol_width) const noexcept
{
    if(board_type != Board_Type::MUSKETEER)
    {
        return;
    }

    std::cout << std::setw(indentation) << std::left << "" << std::right;
    for(auto piece : gated_pieces[static_cast<int>(color)])
    {
        if(piece)
        {
            std::cout << std::setw(symbol_width) << piece.fen_symbol();
        }
        else
        {
            std::cout << std::setw(symbol_width) << '#';
        }
    }
    std::cout << std::endl;
}

void Board::ascii_draw_below_board(int indentation, int symbol_width) const noexcept
{
    ascii_draw_gate(Piece_Color::WHITE, indentation, symbol_width);
}

Square_Difference Board::check_origin() const noexcept
{
    auto checks = moves_attacking_square(find_king(opposite(whose_turn())), whose_turn());
    for(size_t i = 0; i < checks.size(); ++i)
    {
        if(checks[i])
        {
            return Move::attack_direction_from_index(i);
        }
    }
    return {0, 0};
}

void Board::other_move_effects(const Move& move) noexcept
{
    auto base_rank = whose_turn() == Piece_Color::WHITE ? 1 : 8;
    if(move.start().rank() == base_rank)
    {
        auto color_index = static_cast<int>(whose_turn());
        auto file_index = move.start().file() - 'a';
        auto gated_piece = gated_pieces[color_index][file_index];
        if(gated_piece)
        {
            place_piece(gated_piece, move.start());
            gated_pieces[color_index][file_index] = {};
        }
    }
}

void Board::set_initial_fen(const std::string& fen) noexcept
{
    starting_fen = fen;
}

void Board::set_unmoved_gate_guardians() noexcept
{
    for(auto base_rank : {1, 8})
    {
        for(auto file = 'a'; file <= 'h'; ++file)
        {
            auto gate_index = file - 'a';
            const auto& player_gated_pieces = gated_pieces[base_rank == 1 ? 0 : 1];
            if(player_gated_pieces[gate_index])
            {
                set_unmoved({file, base_rank});
            }
        }
    }
}

void Board::pick_and_place_random_gated_pieces() noexcept
{
    static const auto choices = std::array<Piece_Type, 10>{Piece_Type::LEOPARD,
                                                           Piece_Type::CANNON,
                                                           Piece_Type::UNICORN,
                                                           Piece_Type::DRAGON,
                                                           Piece_Type::CHANCELLOR,
                                                           Piece_Type::ARCHBISHOP,
                                                           Piece_Type::ELEPHANT,
                                                           Piece_Type::HAWK,
                                                           Piece_Type::FORTRESS,
                                                           Piece_Type::SPIDER};
    auto first_index = Random::random_integer({0}, choices.size() - 1);
    auto second_index = Random::random_integer({0}, choices.size() - 2);
    if(second_index >= first_index)
    {
        ++second_index;
    }
    randomly_place_gated_pieces(choices[first_index], choices[second_index]);
}

void Board::randomly_place_gated_pieces(Piece_Type first_gated_piece, Piece_Type second_gated_piece) noexcept
{
    gated_piece_types = {first_gated_piece, second_gated_piece};
    for(auto color : {Piece_Color::WHITE, Piece_Color::BLACK})
    {
        auto& gate_list = gated_pieces[static_cast<int>(color)];
        for(auto type : gated_piece_types)
        {
            auto gated_piece = Piece{color, type};
            if(std::find(gate_list.begin(), gate_list.end(), gated_piece) != gate_list.end())
            {
                continue;
            }

            auto other_index = std::distance(gate_list.begin(),
                                             std::find_if(gate_list.begin(), gate_list.end(),
                                                          [](const auto& piece)
                                                          {
                                                              return bool(piece);
                                                          }));

            while(true)
            {
                auto file_index = Random::random_integer(0, 7);
                if(file_index == other_index)
                {
                    continue;
                }

                // Cannot place gated pieces behind rook and king
                if(file_index == 4 && (other_index == 0 || other_index == 7))
                {
                    continue;
                }

                // Cannot place gated pieces behind rook and king
                if(other_index == 4 && (file_index == 0 || file_index == 7))
                {
                    continue;
                }

                gate_list[file_index] = gated_piece;
                break;
            }
        }
    }
}
