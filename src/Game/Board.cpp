#include "Game/Board.h"

#include <iostream>
#include <fstream>
#include <cctype>
#include <cassert>
#include <stdexcept>
#include <array>
#include <mutex>
#include <algorithm>
#include <bitset>
#include <string>
#include <chrono>
using namespace std::chrono_literals;
#include <map>
#include <bit>
#include <type_traits>
#include <functional>
#include <utility>
#include <sstream>
#include <print>

#include "Game/Clock.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"
#include "Game/Move.h"
#include "Game/PGN.h"

#include "Players/Player.h"

#include "Utility/Exceptions.h"
#include "Utility/Random.h"
#include "Utility/String.h"
#include "Utility/Fixed_Capacity_Vector.h"
#include "Utility/Algorithm.h"

namespace
{
    const auto square_hash_values =
    []()
    {
        // One entry for each piece on each square (including no piece)
        std::array<std::array<uint64_t, 13>, 64> hash_cache;
        for(auto& square_indexed_row : hash_cache)
        {
            std::ranges::generate(square_indexed_row, Random::random_unsigned_int64);
        }
        return hash_cache;
    }();

    const auto castling_hash_values =
    []()
    {
        std::array<std::array<uint64_t, 2>, 2> castling_hash_cache; // Indexed by [Piece_Color][Direction]
        for(auto& a : castling_hash_cache)
        {
            std::ranges::generate(a, Random::random_unsigned_int64);
        }
        return castling_hash_cache;
    }();

    const auto switch_turn_board_hash = Random::random_unsigned_int64();
    const auto en_passant_hash_values =
    []()
    {
        std::array<uint64_t, 8> en_passant_hash_cache;
        std::ranges::generate(en_passant_hash_cache, Random::random_unsigned_int64);
        return en_passant_hash_cache;
    }();

    const std::string standard_starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    //! \brief An anonymous class for storing FENs by associating them with the Zobrist hash of the board created by the FEN.
    //! 
    //! The purpose of this class is to store FEN strings outside of the Board class so that they are not uselessly copied during games.
    class
    {
        public:
            void add(uint64_t board_hash, const std::string& fen) noexcept
            {
                const auto map_lock = std::lock_guard(starting_fen_map_lock);
                starting_fen_from_starting_hash[board_hash] = String::remove_extra_whitespace(fen);
            }

            std::string retrieve(uint64_t board_hash) const noexcept
            {
                const auto map_lock = std::lock_guard(starting_fen_map_lock);
                return starting_fen_from_starting_hash.at(board_hash);
            }

        private:
            mutable std::mutex starting_fen_map_lock;
            std::map<uint64_t, std::string> starting_fen_from_starting_hash;
    } fen_cache;

    template<typename... Format_Args>
    void fen_parse_assert(bool assertion, const std::string& input_fen, const std::string& failure_message_template, Format_Args... args)
    {
        if( ! assertion)
        {
            throw std::invalid_argument(std::format("Bad FEN input: {}\n{}", input_fen,
                                                    String::sformat(failure_message_template, args...)));
        }
    }
}

Board::Board() noexcept : Board(standard_starting_fen)
{
}

Board::Board(const std::string& input_fen)
{
    const auto fen_parse = String::split(input_fen);
    fen_parse_assert(fen_parse.size() == 6, input_fen, "Wrong number of fields (should be 6)");

    const auto board_parse = String::split(fen_parse.at(0), "/");
    fen_parse_assert(board_parse.size() == 8, input_fen, "Board has wrong number of rows (should be 8)");

    for(int rank = 8; rank >= 1; --rank)
    {
        char file = 'a';
        for(const auto symbol : board_parse.at(size_t(8) - rank))
        {
            if(std::isdigit(symbol))
            {
                file += char(symbol - '0');
                fen_parse_assert(file <= 'h' + 1, input_fen, "Too many squares in rank {}", rank);
            }
            else
            {
                fen_parse_assert(file <= 'h', input_fen, "Too many squares in rank {}", rank);
                const auto piece = Piece{symbol};
                fen_parse_assert(piece.type() != Piece_Type::PAWN || (rank != 1 && rank != 8), input_fen, "Pawns cannot be placed on the home ranks.");
                fen_parse_assert(piece.type() != Piece_Type::KING || ! find_king(piece.color()).is_set(), input_fen, 
                                 "More than one {} king.", String::lowercase(color_text(piece.color())));

                place_piece(piece, {file, rank});
                ++file;
            }
        }

        fen_parse_assert(file == 'h' + 1, input_fen, "Too few squares in rank {}", rank);
    }

    fen_parse_assert(find_king(Piece_Color::WHITE).is_set(), input_fen, "White king not in FEN string");
    fen_parse_assert(find_king(Piece_Color::BLACK).is_set(), input_fen, "Black king not in FEN string");

    const auto first_turn = fen_parse.at(1);
    fen_parse_assert(first_turn == "w" || first_turn == "b", input_fen, "Invalid character for whose turn: {}", first_turn);
    if(first_turn == "b")
    {
        switch_turn();
    }

    const auto non_turn_color = opposite(whose_turn());
    fen_parse_assert(safe_for_king(find_king(non_turn_color), non_turn_color), input_fen,
                     "{} is in check but it is {}'s turn.", color_text(non_turn_color), color_text(whose_turn()));

    const auto castling_parse = fen_parse.at(2);
    if(castling_parse != "-")
    {
        for(const auto c : castling_parse)
        {
            fen_parse_assert(std::string_view{"KQkq"}.contains(c), input_fen, "Illegal character in castling section: {} ({})", c, castling_parse);

            const auto piece_color = std::isupper(c) ? Piece_Color::WHITE : Piece_Color::BLACK;
            const auto rook_square = Square{std::toupper(c) == 'K' ? 'h' : 'a', std::isupper(c) ? 1 : 8};
            const auto king_square = Square{'e', rook_square.rank()};
            const auto castling_side = rook_square.file() == 'a' ? Direction::LEFT : Direction::RIGHT;

            const auto side = std::string{castling_side == Direction::RIGHT ? "king" : "queen"};
            const auto rook = Piece{piece_color, Piece_Type::ROOK};
            const auto king = Piece{piece_color, Piece_Type::KING};

            fen_parse_assert(piece_on_square(rook_square) == rook, input_fen,
                             "There must be a {} rook on {} to castle {}side.", String::lowercase(color_text(piece_color)), rook_square.text(), side);
            fen_parse_assert(piece_on_square(king_square) == king, input_fen,
                             "There must be a {} king on {} to castle.", String::lowercase(color_text(piece_color)), king_square.text());
            fen_parse_assert( ! castle_is_legal(piece_color, castling_side), input_fen,
                             "There are repeated characters in the castling text: {}", castling_parse);

            make_castle_legal(piece_color, castling_side);
        }
    }

    const auto en_passant_parse = fen_parse.at(3);
    if(en_passant_parse != "-")
    {
        fen_parse_assert(en_passant_parse.size() == 2, input_fen, "Invalid en passant square.");

        make_en_passant_targetable({en_passant_parse[0], en_passant_parse[1] - '0'});

        fen_parse_assert(en_passant_target.is_set(), input_fen, "Invalid en passant square.");
        fen_parse_assert( ! piece_on_square(en_passant_target), input_fen, "Piece is not allowed on en passant target square.");

        const auto last_move_pawn = piece_on_square(en_passant_target + Square_Difference{0, whose_turn() == Piece_Color::WHITE ? -1 : 1});
        fen_parse_assert(last_move_pawn == Piece{opposite(whose_turn()), Piece_Type::PAWN}, input_fen, "There must be a pawn past the en passant target square.");
        fen_parse_assert(en_passant_target.rank() == (whose_turn() == Piece_Color::WHITE ? 6 : 3), input_fen, "The en passant target must be on the third rank (if black to move) or sixth rank (if white to move).");
    }

    // Fill repeat counter to indicate moves since last
    // pawn move or capture.
    const auto fifty_move_count_input = String::to_number<size_t>(fen_parse.at(4));
    fen_parse_assert(fifty_move_count_input < repeat_count.maximum_size(), input_fen, "Halfmove clock value too large.");
    add_board_position_to_repeat_record();
    while(moves_since_pawn_or_capture() < fifty_move_count_input)
    {
        add_to_repeat_count(Random::random_unsigned_int64());
    }

    const auto first_full_move_label = String::to_number<size_t>(fen_parse.at(5));
    plies_at_construction = 2*(first_full_move_label - 1) + (whose_turn() == Piece_Color::WHITE ? 0 : 1);

    recreate_move_caches();

    starting_hash = board_hash();
    fen_cache.add(starting_hash, fen());
    fen_parse_assert(fen() == original_fen(), input_fen, "Result: {}", fen());
}

Piece& Board::piece_on_square(const Square square) noexcept
{
    return board[square.index()];
}

Piece Board::piece_on_square(const Square square) const noexcept
{
    return board[square.index()];
}

bool Board::is_in_legal_moves_list(const Move& move) const noexcept
{
    return std::ranges::find(legal_moves(), &move) != legal_moves().end();
}

std::string Board::fen() const noexcept
{
    std::vector<std::string> rows;
    for(int rank = 8; rank >= 1; --rank)
    {
        rows.push_back("");
        int empty_count = 0;
        for(char file = 'a'; file <= 'h'; ++file)
        {
            const auto piece = piece_on_square({file, rank});
            if( ! piece)
            {
                ++empty_count;
            }
            else
            {
                if(empty_count > 0)
                {
                    rows.back() += std::to_string(empty_count);
                    empty_count = 0;
                }
                rows.back() += piece.fen_symbol();
            }
        }

        if(empty_count > 0)
        {
            rows.back() += std::to_string(empty_count);
        }
    }
    auto fen_parts = std::vector<std::string>{String::join(rows, "/")};

    fen_parts.push_back(String::lowercase(color_text(whose_turn()).substr(0, 1)));

    std::string castling_mark;
    for(const auto player : {Piece_Color::WHITE, Piece_Color::BLACK})
    {
        for(const auto direction : {Direction::RIGHT, Direction::LEFT})
        {
            if(castle_is_legal(player, direction))
            {
                const auto mark = (direction == Direction::RIGHT ? 'K' : 'Q');
                castling_mark.push_back(player == Piece_Color::BLACK ? String::tolower(mark) : mark);
            }
        }
    }
    fen_parts.push_back(castling_mark.empty() ? "-" : castling_mark);

    fen_parts.push_back(en_passant_target.is_set() ? en_passant_target.text() : unused_en_passant_target.text());
    fen_parts.push_back(std::to_string(moves_since_pawn_or_capture()));
    fen_parts.push_back(std::to_string(1 + all_ply_count()/2));
    return String::join(fen_parts, " ");
}

void Board::cli_print(std::ostream& output) const noexcept
{
    for(auto rank = 8; rank >= 1; --rank)
    {
        std::println(output, "");
        for(auto file = 'a'; file <= 'h'; ++file)
        {
            const auto piece = piece_on_square({file, rank});
            std::print(output, " {}", piece ? piece.fen_symbol() : '.');
        }
    }
    std::println(output, "    {} to move", color_text(whose_turn()));
}

void Board::cli_print_game(const Player& white, const Player& black, const Clock& clock) const noexcept
{
    const auto print_name = [&clock](const auto& player, const auto color)
                            {
                                std::println("\n{} | {}", player.name(), clock.time_left_display(color));
                            };
    print_name(black, Piece_Color::BLACK);
    cli_print(std::cout);
    print_name(white, Piece_Color::WHITE);
    std::println("\n     = = = =");
}

std::string Board::original_fen() const noexcept
{
    return fen_cache.retrieve(starting_hash);
}

Game_Result Board::play_move(const Move& move) noexcept
{
    if(repeat_count.full())
    {
        repeat_count.shift_left();
    }
    update_board(move);
    return move_result();
}

Game_Result Board::play_move(const std::string& move)
{
    return play_move(interpret_move(move));
}

size_t Board::all_ply_count() const noexcept
{
    return plies_at_construction + played_ply_count();
}

std::vector<const Move*> Board::derive_moves(const Board& new_board) const noexcept
{
    const size_t moves_to_derive_count = new_board.all_ply_count() - all_ply_count();;
    if(moves_to_derive_count > 2 || moves_to_derive_count < 1)
    {
        return {};
    }

    for(const auto first_move : legal_moves())
    {
        auto first_move_board = *this;
        first_move_board.play_move(*first_move);
        if(moves_to_derive_count == 1)
        {
            if(first_move_board.fen() == new_board.fen())
            {
                return {first_move};
            }
        }
        else
        {
            const auto next_moves = first_move_board.derive_moves(new_board);
            if(next_moves.size() == 1)
            {
                return {first_move, next_moves.front()};
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

void Board::make_castle_legal(Piece_Color color, Direction direction) noexcept
{
    legal_castles[std::to_underlying(color)][std::to_underlying(direction)] = true;
    current_board_hash ^= castling_hash_values[std::to_underlying(color)][std::to_underlying(direction)];
}

void Board::make_castle_illegal(Piece_Color color, Direction direction) noexcept
{
    if(castle_is_legal(color, direction))
    {
        legal_castles[std::to_underlying(color)][std::to_underlying(direction)] = false;
        current_board_hash ^= castling_hash_values[std::to_underlying(color)][std::to_underlying(direction)];
    }
}

void Board::update_board(const Move& move) noexcept
{
    assert(is_in_legal_moves_list(move));

    ++game_move_count;
    previous_move = &move;
    if(move.is_en_passant(*this))
    {
        remove_piece({move.end().file(), move.start().rank()});
    }
    move_piece(move);
    clear_en_passant_target();
    unused_en_passant_target = {};
    move.side_effects(*this);
    switch_turn();

    recreate_move_caches();

    add_board_position_to_repeat_record();
}

bool Board::castle_is_legal(Piece_Color color, Direction direction) const noexcept
{
    return legal_castles[std::to_underlying(color)][std::to_underlying(direction)];
}

void Board::switch_turn() noexcept
{
    turn_color = opposite(turn_color);
    update_whose_turn_hash();
}

size_t Board::played_ply_count() const noexcept
{
    return game_move_count;
}

const Move* Board::last_move() const noexcept
{
    return previous_move;
}

int Board::castling_direction(Piece_Color player) const noexcept
{
    return castling_movement[std::to_underlying(player)];
}

const Move& Board::interpret_move(const std::string& move_text) const
{
    constexpr static auto end_marks = "+#?!";
    const auto raw = [](const auto& text)
        {
            const auto mark_location = text.find_first_of(end_marks);
            if(mark_location >= text.size() - 1)
            {
                return text.substr(0, mark_location);
            }
            else
            {
                return text;
            }
        };
    const auto raw_move_text = raw(move_text);
    const auto move_iter =
        std::ranges::find_if(legal_moves(),
                             [this, &raw_move_text, raw](auto move)
                             {
                                 return raw(move->algebraic(*this)) == raw_move_text ||
                                     move->coordinates() == raw_move_text;
                             });
    if(move_iter != legal_moves().end())
    {
        return **move_iter;
    }
    else
    {
        throw Illegal_Move(std::format("The move text is not a valid or legal move: {}", move_text));
    }
}

bool Board::is_legal_move(const std::string& text) const noexcept
{
    try
    {
        interpret_move(text);
        return true;
    }
    catch(const Illegal_Move&)
    {
        return false;
    }
}

void Board::move_piece(const Move& move) noexcept
{
    if(piece_on_square(move.end()))
    {
        remove_piece(move.end());
        clear_repeat_count();
    }

    const auto moving_piece = piece_on_square(move.start());
    if(moving_piece.type() == Piece_Type::PAWN)
    {
        clear_repeat_count();
    }
    else if(moving_piece.type() == Piece_Type::KING)
    {
        make_castle_illegal(moving_piece.color(), Direction::RIGHT);
        make_castle_illegal(moving_piece.color(), Direction::LEFT);
    }
    remove_piece(move.start());
    place_piece(moving_piece, move.end());
}

Piece_Color Board::whose_turn() const noexcept
{
    return turn_color;
}

const std::vector<const Move*>& Board::legal_moves() const noexcept
{
    return legal_moves_cache;
}

bool Board::attacked_by(Square target, Piece_Color attacker) const noexcept
{
    return moves_attacking_square(target, attacker).any();
}

void Board::remove_piece(const Square square) noexcept
{
    const auto leaving_piece = piece_on_square(square);
    if(leaving_piece.type() == Piece_Type::ROOK)
    {
        const auto base_rank = leaving_piece.color() == Piece_Color::WHITE ? 1 : 8;
        const auto right_castle_square = Square{'h', base_rank};
        const auto  left_castle_square = Square{'a', base_rank};
        if(square == right_castle_square)
        {
            make_castle_illegal(leaving_piece.color(), Direction::RIGHT);
        }
        else if(square == left_castle_square)
        {
            make_castle_illegal(leaving_piece.color(), Direction::LEFT);
        }
    }
    place_piece({}, square);
}

void Board::place_piece(const Piece piece, const Square square) noexcept
{
    update_board_hash(square); // XOR out piece on square

    const auto old_piece = piece_on_square(square);
    piece_on_square(square) = piece;

    remove_attacks_from(square, old_piece);
    update_blocks(square, old_piece, piece);
    add_attacks_from(square, piece);

    update_board_hash(square); // XOR in new piece on square

    if(piece && piece.type() == Piece_Type::KING)
    {
        record_king_location(piece.color(), square);
    }
}

void Board::record_king_location(const Piece_Color color, const Square square)
{
    king_location[std::to_underlying(color)] = square;
}

void Board::add_attacks_from(const Square square, const Piece piece) noexcept
{
    modify_attacks(square, piece, true);
}

void Board::modify_attacks(const Square square, const Piece piece, const bool adding_attacks) noexcept
{
    if( ! piece)
    {
        return;
    }

    const auto attacking_color = piece.color();
    const auto vulnerable_king = Piece{opposite(attacking_color), Piece_Type::KING};
    for(const auto& attack_move_list : piece.attacking_move_lists(square))
    {
        for(const auto attack : attack_move_list)
        {
            const auto attacked_square = attack->end();
            const auto attacked_index = attacked_square.index();

            potential_attacks[std::to_underlying(attacking_color)][attacked_index][attack->attack_index()] = adding_attacks;

            const auto blocking_piece = piece_on_square(attacked_square);
            if(blocking_piece && blocking_piece != vulnerable_king)
            {
                break;
            }
        }
    }
}

void Board::remove_attacks_from(const Square square, const Piece old_piece) noexcept
{
    modify_attacks(square, old_piece, false);
}

void Board::update_blocks(const Square square, const Piece old_piece, const Piece new_piece) noexcept
{
    // Replacing nothing with nothing changes nothing.
    // Replacing one piece with another does not change which
    // moves are blocked. Only happens during pawn promotions.
    if(bool(old_piece) == bool(new_piece))
    {
        return;
    }

    const auto add_new_attacks = ! new_piece; // New pieces block; no new pieces allow new moves through
    const auto origin_square_index = square.index();

    for(const auto attacking_color : {Piece_Color::WHITE, Piece_Color::BLACK})
    {
        const auto vulnerable_king = Piece{opposite(attacking_color), Piece_Type::KING};
        if(new_piece == vulnerable_king)
        {
            continue;
        }

        const auto& attack_direction_list = potential_attacks[std::to_underlying(attacking_color)][origin_square_index];
        for(size_t index = 0; index < 8; ++index) // < 8 to exclude knight moves, which are never blocked
        {
            if(attack_direction_list[index])
            {
                const auto step = Move::attack_direction_from_index(index);
                const auto revealed_attacker = piece_on_square(square - step);
                if(revealed_attacker && (revealed_attacker.type() == Piece_Type::PAWN || revealed_attacker.type() == Piece_Type::KING))
                {
                    continue; // Pawns and kings are never blocked
                }

                for(const auto target_square : Square::square_line_from(square, step))
                {
                    potential_attacks[std::to_underlying(attacking_color)][target_square.index()][index] = add_new_attacks;

                    const auto piece = piece_on_square(target_square);
                    if(piece && piece != vulnerable_king)
                    {
                        break;
                    }
                }
            }
        }
    }
}

std::bitset<16> Board::moves_attacking_square(const Square square, const Piece_Color attacking_color) const noexcept
{
    return potential_attacks[std::to_underlying(attacking_color)][square.index()];
}

std::bitset<16> Board::checking_moves() const noexcept
{
    return moves_attacking_square(find_king(whose_turn()), opposite(whose_turn()));
}

bool Board::king_is_in_check() const noexcept
{
    return attacked_by(find_king(whose_turn()), opposite(whose_turn()));
}

bool Board::safe_for_king(const Square square, const Piece_Color king_color) const noexcept
{
    return ! attacked_by(square, opposite(king_color));
}

bool Board::king_is_in_check_after_move(const Move& move) const noexcept
{
    const auto king_square = find_king(whose_turn());
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
        if(in_line_in_order(checking_square, move.end(), king_square))
        {
            return piece_is_pinned(move.start());
        }

        if(move.is_en_passant(*this)) 
        {
            const auto captured_pawn_square = Square{move.end().file(), move.start().rank()};
            return checking_square != captured_pawn_square || piece_is_pinned(move.start());
        }

        // Nothing is done about the check
        return true;
    }

    if(piece_is_pinned(move.start()))
    {
        return ! moves_are_parallel(move.movement(), king_square - move.start());
    }

    if(move.is_en_passant(*this) && king_square.rank() == move.start().rank())
    {
        const auto squares = Square::square_line_from(king_square, (move.start() - king_square).step());
        const auto rook = Piece{opposite(whose_turn()), Piece_Type::ROOK};
        const auto queen = Piece{opposite(whose_turn()), Piece_Type::QUEEN};
        const auto revealed_attacker =
            std::ranges::find_if(squares,
                                 [this, rook, queen](auto square)
                                 {
                                     const auto piece = piece_on_square(square);
                                     return piece == rook || piece == queen;
                                 });

        return revealed_attacker != squares.end() &&
               Algorithm::has_exactly_n(squares.begin(), revealed_attacker, [this](auto square) { return piece_on_square(square); }, 2);
    }

    return false;
}

bool Board::move_checks_king(const Move& move) const noexcept
{
    const auto opponent_king_square = find_king(opposite(whose_turn()));
    const auto moving_piece = piece_on_square(move.start());
    const auto piece = move.promotion() ? move.promotion() : moving_piece;
    const auto& after_moves = piece.attacking_move_lists(move.end());
    for(const auto& move_list : after_moves)
    {
        const auto checking_move = std::find_if(move_list.begin(),
                                                move_list.end(),
                                                [opponent_king_square](auto attack)
                                                {
                                                    return attack->end() == opponent_king_square;
                                                });
        if(checking_move != move_list.end())
        {
            const auto found_move = *checking_move;
            return piece.type() == Piece_Type::KNIGHT || all_empty_between(found_move->start(),
                                                                           found_move->end());
        }
    }

    return false;
}

bool Board::no_legal_moves() const noexcept
{
    return legal_moves().empty();
}

void Board::make_en_passant_targetable(const Square square) noexcept
{
    if(en_passant_target.is_set())
    {
        current_board_hash ^= en_passant_hash_values[en_passant_target.file() - 'a'];
    }

    if(square.is_set())
    {
        current_board_hash ^= en_passant_hash_values[square.file() - 'a'];
    }

    en_passant_target = square;
}

bool Board::is_en_passant_targetable(const Square square) const noexcept
{
    return en_passant_target == square;
}

void Board::clear_en_passant_target() noexcept
{
    make_en_passant_targetable({});
}

Square Board::find_king(const Piece_Color color) const noexcept
{
    return king_location[std::to_underlying(color)];
}

void Board::recreate_move_caches() noexcept
{
    checking_square = find_checking_square();
    legal_moves_cache.clear();
    for(const auto square : Square::all_squares())
    {
        const auto piece = piece_on_square(square);
        if(piece && piece.color() == whose_turn())
        {
            for(const auto& move_list : piece.move_lists(square))
            {
                for(const auto move : move_list)
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

    if(en_passant_target.is_set() && std::ranges::none_of(legal_moves_cache, [this](const auto move) { return move->is_en_passant(*this); }))
    {
        disable_en_passant_target();
    }
}

void Board::disable_en_passant_target() noexcept
{
    unused_en_passant_target = en_passant_target;
    clear_en_passant_target();
}

Square Board::find_checking_square() const noexcept
{
    if( ! king_is_in_check())
    {
        return {};
    }

    const auto checking_index = std::countr_zero(checking_moves().to_ulong());
    const auto step = Move::attack_direction_from_index(checking_index);
    const auto king_square = find_king(whose_turn());
    const auto squares = Square::square_line_from(king_square, -step);
    return *std::ranges::find_if(squares, [this](auto square) { return piece_on_square(square); });
}

bool Board::enough_material_to_checkmate(const Piece_Color piece_color) const noexcept
{
    auto piece_is_right = [piece_color](const auto piece, const auto type) { return piece == Piece{piece_color, type}; };

    if(std::ranges::any_of(board,
                           [piece_is_right](const auto piece)
                           {
                               return piece_is_right(piece, Piece_Type::QUEEN) ||
                                   piece_is_right(piece, Piece_Type::ROOK) ||
                                   piece_is_right(piece, Piece_Type::PAWN);
                           }))
    {
        return true;
    }

    const auto knight_count = std::ranges::count_if(board, [piece_is_right](const auto piece) { return piece_is_right(piece, Piece_Type::KNIGHT); });
    if(knight_count > 1)
    {
        return true;
    }

    auto bishop_on_square_color =
        [this, piece_is_right](const Square_Color square_color, const Square square)
        {
            return piece_is_right(piece_on_square(square), Piece_Type::BISHOP) && square.color() == square_color;
        };

    const auto squares = Square::all_squares();
    const auto bishops_on_white =
        std::ranges::any_of(squares, [bishop_on_square_color](const auto square) { return bishop_on_square_color(Square_Color::WHITE, square); });
    const auto bishops_on_black =
        std::ranges::any_of(squares, [bishop_on_square_color](const auto square) { return bishop_on_square_color(Square_Color::BLACK, square); });
    return (bishops_on_white && bishops_on_black) || (knight_count > 0 && (bishops_on_white || bishops_on_black));
}

bool Board::enough_material_to_checkmate() const noexcept
{
    auto piece_is_right = [](auto piece, auto type) { return piece && piece.type() == type; };

    if(std::ranges::any_of(board,
                           [piece_is_right](const auto piece)
                           {
                               return piece_is_right(piece, Piece_Type::QUEEN) ||
                                   piece_is_right(piece, Piece_Type::ROOK) ||
                                   piece_is_right(piece, Piece_Type::PAWN);
                           }))
    {
        return true;
    }

    const auto knight_count = std::ranges::count_if(board, [piece_is_right](const auto piece) { return piece_is_right(piece, Piece_Type::KNIGHT); });
    if(knight_count > 1)
    {
        return true;
    }

    auto bishop_on_square_color =
        [this, piece_is_right](const Square_Color square_color, const Square square)
        {
            return piece_is_right(piece_on_square(square), Piece_Type::BISHOP) && square.color() == square_color;
        };

    const auto squares = Square::all_squares();
    const auto bishops_on_white =
        std::ranges::any_of(squares, [bishop_on_square_color](const auto square) { return bishop_on_square_color(Square_Color::WHITE, square); });
    const auto bishops_on_black =
        std::ranges::any_of(squares, [bishop_on_square_color](const auto square) { return bishop_on_square_color(Square_Color::BLACK, square); });
    return (bishops_on_white && bishops_on_black) || (knight_count > 0 && (bishops_on_white || bishops_on_black));
}

void Board::update_board_hash(const Square square) noexcept
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
    return square_hash_values[square.index()][piece_on_square(square).index()];
}

uint64_t Board::board_hash() const noexcept
{
    return current_board_hash;
}

bool Board::move_captures(const Move& move) const noexcept
{
    const auto attacked_piece = piece_on_square(move.end());

    assert(is_in_legal_moves_list(move));
    assert( ! attacked_piece || (move.can_capture() && attacked_piece.color() == opposite(whose_turn())));

    return attacked_piece || move.is_en_passant(*this);
}

bool Board::move_changes_material(const Move& move) const noexcept
{
    return move_captures(move) || move.promotion_piece_symbol();
}

bool Board::king_multiply_checked() const noexcept
{
    return checking_moves().count() > 1;
}

bool Board::all_empty_between(const Square start, const Square end) const noexcept
{
    assert(straight_line_move(start, end));
    const auto squares = Square::squares_between(start, end);
    return std::ranges::none_of(squares, [this](auto square) { return piece_on_square(square); });
}

bool Board::piece_is_pinned(const Square square) const noexcept
{
    return piece_is_pinned_to_king(whose_turn(), square);
}

bool Board::is_discovered_check(const Move& move) const noexcept
{
    assert(is_in_legal_moves_list(move));
    const auto king_color = opposite(whose_turn());
    const auto king_square = find_king(king_color);
    const auto direction = king_square - move.start();
    return ! moves_are_parallel(direction, move.movement()) && piece_is_pinned_to_king(king_color, move.start());
}

bool Board::piece_is_pinned_to_king(const Piece_Color king_color, const Square square) const noexcept
{
    const auto king_square = find_king(king_color);
    if(king_square == square || ! straight_line_move(square, king_square))
    {
        return false;
    }

    const auto direction = (king_square - square).step();
    const auto attack_square = square - direction;
    const auto attacking_color = opposite(king_color);
    const auto color_index = std::to_underlying(attacking_color);
    const auto opponent_pawn = Piece(attacking_color, Piece_Type::PAWN);
    const auto opponent_king = Piece(attacking_color, Piece_Type::KING);
    return potential_attacks[color_index][square.index()][Move::attack_index(direction)]
        && piece_on_square(attack_square) != opponent_pawn
        && piece_on_square(attack_square) != opponent_king
        && all_empty_between(king_square, square);
}

void Board::add_board_position_to_repeat_record() noexcept
{
    add_to_repeat_count(board_hash());
}

void Board::add_to_repeat_count(const uint64_t new_hash) noexcept
{
    repeat_count.push_back(new_hash);
}

ptrdiff_t Board::current_board_position_repeat_count() const noexcept
{
    return std::ranges::count(repeat_count, board_hash());
}

size_t Board::moves_since_pawn_or_capture() const noexcept
{
    return repeat_count.size() - 1;
}

std::ptrdiff_t Board::repeat_count_from_depth(size_t depth) const noexcept
{
    depth = std::min(depth, repeat_count.size() - 1);
    return std::count(repeat_count.end() - depth - 1, repeat_count.end(), board_hash());
}

void Board::clear_repeat_count() noexcept
{
    repeat_count.clear();
}

size_t Board::castling_move_index(const Piece_Color player) const noexcept
{
    return castling_index[std::to_underlying(player)];
}

bool Board::player_castled(const Piece_Color player) const noexcept
{
    return castling_direction(player) != 0;
}

Board Board::without_random_pawn() const noexcept
{
    assert(std::ranges::any_of(board, [](auto p) { return p && p.type() == Piece_Type::PAWN; }));

    auto result = *this;
    while(true)
    {
        const auto square = Square('a' + char(Random::random_integer(0, 7)), Random::random_integer(1, 8));
        const auto piece = result.piece_on_square(square);
        if(piece && piece.type() == Piece_Type::PAWN)
        {
            result.remove_piece(square);
            return result;
        }
    }
}

std::vector<const Move*> Board::quiescent(const std::array<double, 6>& piece_values) const noexcept
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
    while(current_board.attacked_by(square, current_board.whose_turn()))
    {
        std::vector<const Move*> capturing_moves;
        std::ranges::copy_if(current_board.legal_moves(), std::back_inserter(capturing_moves), [square](auto move) { return move->end() == square; });

        if(capturing_moves.empty())
        {
            // This can happen when the only attacking piece is a king
            // and the attacked piece is guarded.
            break;
        }

        // Attack with the weakest piece first
        const auto move =
            *std::ranges::min_element(capturing_moves,
                                      [&piece_values, &current_board](auto move1, auto move2)
                                      {
                                          return piece_values[std::to_underlying(current_board.piece_on_square(move1->start()).type())] <
                                              piece_values[std::to_underlying(current_board.piece_on_square(move2->start()).type())];
                                      });

        // Make sure that an exchange does not lose material
        const auto moving_piece = current_board.piece_on_square(move->start());
        const auto attacked_piece = current_board.piece_on_square(move->end());
        current_board.play_move(*move);
        capture_moves.push_back(move);
        state_values.push_back(state_values.back() + (moving_piece.color() == player_color ? +1 : -1)*piece_values[std::to_underlying(attacked_piece.type())]);
    }


    if(capture_moves.empty())
    {
        return capture_moves;
    }

    // Make sure to stop before either player ends up in an
    // avoidable loss of material.
    auto minimax_iter = state_values.rbegin();
    auto minimax_value = *minimax_iter;
    for(auto iter = std::next(minimax_iter); iter != state_values.rend(); std::advance(iter, 1))
    {
        auto value = *iter;

        // Even indices indicate a player's choice (maximize score).
        // Odd indices indicate an opponent's choice (minimize score).
        const auto index = std::distance(iter, state_values.rend()) - 1;
        if(index % 2 == 0)
        {
            if(value > minimax_value)
            {
                minimax_value = value;
                minimax_iter = iter;
            }
        }
        else
        {
            if(value < minimax_value)
            {
                minimax_value = value;
                minimax_iter = iter;
            }
        }
    }

    const auto minimax_index = std::distance(minimax_iter, state_values.rend()) - 1;
    return {capture_moves.begin(), capture_moves.begin() + minimax_index};
}

void Board::compare_hashes(const Board& other) const noexcept
{
    std::print(std::cerr, "Differing square hashes: ");
    for(const auto square : Square::all_squares())
    {
        if(square_hash(square) != other.square_hash(square))
        {
            std::print(std::cerr, " {}", square.text());
        }
    }
    std::print(std::cerr, "\n");
    const auto hash_diff = (board_hash() ^ other.board_hash());
    constexpr auto white = std::to_underlying(Piece_Color::WHITE);
    constexpr auto black = std::to_underlying(Piece_Color::BLACK);
    constexpr auto right = std::to_underlying(Direction::RIGHT);
    constexpr auto left  = std::to_underlying(Direction::LEFT);
    const auto both_white_castles = (castling_hash_values[white][right] ^ castling_hash_values[white][left]);
    const auto both_black_castles = (castling_hash_values[black][right] ^ castling_hash_values[black][left]);
    if(en_passant_target.is_set() && hash_diff == en_passant_hash_values[en_passant_target.file() - 'a'])
    {
        std::println(std::cerr, "en passant hash");
    }
    else if(hash_diff == both_white_castles)
    {
        std::println(std::cerr, "both white castling");
    }
    else if(hash_diff == both_black_castles)
    {
        std::println(std::cerr, "both black castling");
    }
    else if(hash_diff == castling_hash_values[black][right])
    {
        std::println(std::cerr, "black kingside castle");
    }
    else if(hash_diff == castling_hash_values[white][right])
    {
        std::println(std::cerr, "white kingside castle");
    }
    else if(hash_diff == castling_hash_values[white][left])
    {
        std::println(std::cerr, "white queenside castle");
    }
    else if(hash_diff == castling_hash_values[black][left])
    {
        std::println(std::cerr, "black queenside castle");
    }
    else
    {
        std::println(std::cerr, "Something else");
    }
}
