#include <iostream>
#include <fstream>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cassert>
#include <stdexcept>
#include <array>
#include <mutex>
#include <algorithm>
#include <sstream>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"

#include "Moves/Move.h"
#include "Moves/Threat_Generator.h"
#include "Moves/Threat_Iterator.h"

#include "Players/Player.h"

#include "Exceptions/Illegal_Move.h"
#include "Exceptions/Promotion_Piece_Needed.h"

#include "Utility/Math.h"
#include "Utility/Random.h"
#include "Utility/String.h"

// NOTE: The pawn must be created last since it takes pointers to the other pieces
const Piece Board::white_rook(WHITE, ROOK);
const Piece Board::white_knight(WHITE, KNIGHT);
const Piece Board::white_bishop(WHITE, BISHOP);
const Piece Board::white_queen(WHITE, QUEEN);
const Piece Board::white_king(WHITE, KING);
const Piece Board::white_pawn(WHITE, PAWN);

const Piece Board::black_rook(BLACK, ROOK);
const Piece Board::black_knight(BLACK, KNIGHT);
const Piece Board::black_bishop(BLACK, BISHOP);
const Piece Board::black_queen(BLACK, QUEEN);
const Piece Board::black_king(BLACK, KING);
const Piece Board::black_pawn(BLACK, PAWN);

//! Get a representative of the Piece class corresponding to the requested color and type.

//! Since there is no actual difference between, say, two white bishops, there is only
//! ever one copy of each type of piece and pointers are handed out to all askers. This saves
//! on copying the other internal data of the pieces (namely Move lists).
//! \param piece_type The type of the requested piece (PAWN, ROOK, etc.).
//! \param color The color of the requested piece.
//!        Passing in NONE to the color parameter results in undefined behavior.
//! \returns A pointer to the piece requested.
const Piece* Board::piece_instance(Piece_Type piece_type, Color color)
{
    static const std::array<std::array<const Piece*, 2>, 6> all_pieces =
        {{{{&white_pawn,   &black_pawn  }},
          {{&white_rook,   &black_rook  }},
          {{&white_knight, &black_knight}},
          {{&white_bishop, &black_bishop}},
          {{&white_queen,  &black_queen }},
          {{&white_king,   &black_king  }}}};

    return all_pieces[piece_type][color];
}

std::array<std::array<uint64_t, 13>, 64> Board::square_hash_values{};
std::array<uint64_t, 64> Board::en_passant_hash_values{};
std::array<uint64_t, 64> Board::castling_hash_values{};
uint64_t Board::switch_turn_board_hash;

const std::string Board::standard_starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//! Constructs a board according to an FEN string.

//! \param fen An optional text string given in FEN. If no argument is given,
//!        then the FEN string for the start of a standard chess game is given.
//! \throws std::runtime_error Thrown if the FEN string does not represent a valid board state.
Board::Board(const std::string& fen) :
    board{},
    repeat_count_insertion_point{0},
    unmoved_positions{},
    starting_fen(String::remove_extra_whitespace(fen)),
    pinning_squares{},
    square_searched_for_pin{},
    castling_index{{size_t(-1), size_t(-1)}},
    thinking_indicator(NO_THINKING)
{
    initialize_board_hash();

    auto fen_parse = String::split(fen);
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
        for(const auto& symbol : board_parse.at(8-rank))
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

                Color color = (isupper(symbol) ? WHITE : BLACK);
                switch(toupper(symbol))
                {
                    case 'P':
                        if(rank == 1 || rank == 8)
                        {
                            fen_error("Pawns cannot be placed on the home ranks.");
                        }
                        place_piece(piece_instance(PAWN, color), file, rank);
                        break;
                    case 'R':
                        place_piece(piece_instance(ROOK, color), file, rank);
                        break;
                    case 'N':
                        place_piece(piece_instance(KNIGHT, color), file, rank);
                        break;
                    case 'B':
                        place_piece(piece_instance(BISHOP, color), file, rank);
                        break;
                    case 'Q':
                        place_piece(piece_instance(QUEEN, color), file, rank);
                        break;
                    case 'K':
                        if(king_location[color])
                        {
                            fen_error("More than one " + color_text(color) + " king.");
                        }
                        place_piece(piece_instance(KING, color), file, rank);
                        break;
                    default:
                        fen_error(std::string("Invalid symbol in FEN string: ") + symbol);
                }
                ++file;
            }
        }

        if(file != 'h' + 1)
        {
            fen_error("Too few squares in rank " + std::to_string(rank));
        }
    }

    if( ! find_king(WHITE))
    {
        fen_error("White king not in FEN string");
    }
    if( ! find_king(BLACK))
    {
        fen_error("Black king not in FEN string");
    }

    if(fen_parse[1] == "w")
    {
        turn_color = WHITE;
    }
    else if(fen_parse[1] == "b")
    {
        turn_color = BLACK;
    }
    else
    {
        fen_error("Invalid character for whose turn: " + fen_parse[1]);
    }

    auto non_turn_color = opposite(whose_turn());
    const auto& non_turn_king_square = find_king(non_turn_color);
    if( ! safe_for_king(non_turn_king_square.file(), non_turn_king_square.rank(), non_turn_color))
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
            if(c == '-')
            {
                fen_error("Castling section contains - and other characters: " + castling_parse + ".");
            }

            if( ! String::contains("KQkq", c))
            {
                fen_error(std::string("Illegal character in castling section: ") + c + "(" + castling_parse + ")");
            }

            Color piece_color = std::isupper(c) ? WHITE : BLACK;
            int home_rank = std::isupper(c) ? 1 : 8;

            char rook_file = std::toupper(c) == 'K' ? 'h' : 'a';
            std::string side = std::toupper(c) == 'K' ? "king" : "queen";

            if(piece_on_square(rook_file, home_rank) != piece_instance(ROOK, piece_color))
            {
                fen_error("There must be a " + String::lowercase(color_text(piece_color)) + " rook on " + rook_file + std::to_string(home_rank) + " to castle " + side + "side.");
            }
            set_unmoved(rook_file, home_rank);

            if(piece_on_square('e', home_rank) != piece_instance(KING, piece_color))
            {
                fen_error("There must be a " + String::lowercase(color_text(piece_color)) + " king on e" + std::to_string(home_rank) + " to castle.");
            }
            set_unmoved('e', home_rank);
        }
    }

    auto en_passant_parse = fen_parse.at(3);
    if(en_passant_parse != "-")
    {
        if(en_passant_parse.size() != 2)
        {
            fen_error("Invalid en passant square.");
        }

        make_en_passant_targetable(en_passant_parse[0], en_passant_parse[1] - '0');

        if( ! inside_board(en_passant_target.file(), en_passant_target.rank()))
        {
            fen_error("Invalid en passant square.");
        }

        if(piece_on_square(en_passant_target.file(), en_passant_target.rank()))
        {
            fen_error("Piece is not allowed on en passant target square.");
        }

        auto last_move_pawn = piece_on_square(en_passant_target.file(),
                                              en_passant_target.rank() + (whose_turn() == WHITE ? -1 : 1));
        if(last_move_pawn != piece_instance(PAWN, opposite(whose_turn())))
        {
            fen_error("There must be a pawn past the en passant target square.");
        }
    }

    // Fill repeat counter to indicate moves since last
    // pawn move or capture.
    auto fifty_move_count_input = std::stoi(fen_parse.at(4));
    add_board_position_to_repeat_record();
    while(moves_since_pawn_or_capture() < fifty_move_count_input)
    {
        add_to_repeat_count(Random::random_unsigned_int64());
    }

    move_count_start_offset = std::stoul(fen_parse.at(5));

    if(fen_status() != starting_fen)
    {
        fen_error("Result: " + fen_status());
    }

    recreate_move_caches();

    const auto& king_square = find_king(whose_turn());
    auto attacker_count = 0;
    auto attacking_knight_count = 0;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = piece_on_square(file,rank);
            if( ! piece || piece->color() == whose_turn())
            {
                continue;
            }

            for(auto move : piece->move_list(file, rank))
            {
                if(move->can_capture() &&
                   move->end_file() == king_square.file() &&
                   move->end_rank() == king_square.rank())
                {
                    if(piece->type() == KNIGHT)
                    {
                        ++attacker_count;
                        ++attacking_knight_count;
                    }
                    else if(all_empty_between(file, rank, king_square.file(), king_square.rank()))
                    {
                        ++attacker_count;

                        // Move to next piece since pawns can have multiple
                        // moves with the same start and end squares.
                        break;
                    }
                }
            }
        }
    }

    if(attacker_count > 2)
    {
        fen_error("Too many pieces attacking " + color_text(whose_turn()) + " king.");
    }

    if(attacking_knight_count > 1)
    {
        fen_error("It is impossible for more than one knight to check king.");
    }
}

void Board::fen_error(const std::string& reason) const
{
    throw std::runtime_error("Bad FEN input: " + starting_fen + "\n" + reason);
}

//! Calculates the index for a 64-element array that corresponds to the internal piece storage.

//! \param file File of the queried square.
//! \param rank Rank of the queried square.
//! \returns Index to an array.
//! \throws assert_failure Thrown in DEBUG builds if square coordinates are outside of the board.
size_t Board::square_index(char file, int rank)
{
    assert(inside_board(file, rank));
    return 8*(file - 'a') + (rank - 1);
}

const Piece*& Board::piece_on_square(char file, int rank)
{
    return board[square_index(file, rank)];
}

//! Get the piece on the square indicated by coordinates.

//! \param file File of queried square.
//! \param rank Rank of queried square.
//! \returns Pointer to piece on square. May be nullptr if square is emtpy.
const Piece* Board::piece_on_square(char file, int rank) const
{
    return board[square_index(file, rank)];
}

void Board::set_unmoved(char file, int rank)
{
    update_board_hash(file, rank); // remove reference to moved piece
    unmoved_positions[square_index(file, rank)] = true;
    update_board_hash(file, rank);
}

//! Check that a square coordinate is valid.

//! \param file Square file to check ('a' <= file <= 'h').
//! \param rank Square rank to check (1 <= rank <= 8)
//! \returns Whether the coordinates meet the conditions listed in the parameters above.
bool Board::inside_board(char file, int rank)
{
    return inside_board(file) && inside_board(rank);
}

//! Check that a square file is a valid value.

//! \param file Square file to check ('a' <= file <= 'h')
//! \returns Whether the file meets the condition according to the parameter specification above.
bool Board::inside_board(char file)
{
    return file >= 'a' && file <= 'h';
}

//! Check that a square rank is a valid value.

//! \param rank Square rank to check (1 <= file <= 8)
//! \returns Whether the rank meets the condition according to the parameter specification above.
bool Board::inside_board(int rank)
{
    return rank >= 1 && rank <= 8;
}

//! Returns the color of the square at the input coordinates.

//! This function is useful for Board::enough_material_to_checkmate() since one
//! condition is that bishops on opposite color squares are sufficient to checkmate,
//! while bishops on the same color square are not.
//! \param file Square file.
//! \param rank Square rank.
//! \returns Color of square on board ("a1" --> BLACK; "h1" --> WHITE)
Color Board::square_color(char file, int rank)
{
    return (file - 'a') % 2 == (rank - 1) % 2 ? BLACK : WHITE;
}

//! Checks if there are any legal moves from the start sqaure to the end square.

//! \param file_start File of starting square.
//! \param rank_start Rank of starting square.
//! \param file_end File of ending square.
//! \param rank_end Rank of ending square.
//! \returns True if there is a legal move between the given squares.
bool Board::is_legal(char file_start, int rank_start,
                     char file_end,   int rank_end) const
{
    return std::any_of(legal_moves().begin(), legal_moves().end(),
                       [=](auto move)
                       {
                           return move->start_file() == file_start &&
                                  move->start_rank() == rank_start &&
                                  move->end_file() == file_end &&
                                  move->end_rank() == rank_end;
                       });
}

bool Board::is_in_legal_moves_list(const Move& move) const
{
    return std::find(legal_moves().begin(), legal_moves().end(), &move) != legal_moves().end();
}

std::string Board::fen_status() const
{
    std::string s;

    for(int rank = 8; rank >= 1; --rank)
    {
        int empty_count = 0;
        if(rank < 8)
        {
            s.push_back('/');
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            auto piece = piece_on_square(file, rank);
            if(!piece)
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
                s.push_back(piece->fen_symbol());
            }
        }

        if(empty_count > 0)
        {
            s += std::to_string(empty_count);
        }
    }

    s.push_back(' ');
    s.push_back(whose_turn() == WHITE ? 'w' : 'b');
    s.push_back(' ');

    for(int base_rank : {1, 8})
    {
        if(!piece_has_moved('e', base_rank)) // has king moved?
        {
            for(char rook_file : {'h', 'a'})
            {
                if(!piece_has_moved(rook_file, base_rank)) // have rooks moved
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

    if(en_passant_target)
    {
        s.push_back(en_passant_target.file());
        s += std::to_string(en_passant_target.rank());
    }
    else
    {
        s.push_back('-');
    }

    return s + " " +
        std::to_string(moves_since_pawn_or_capture()) + " " +
        std::to_string(move_count_start_offset + game_record_listing.size()/2);
}

const Move& Board::create_move(char file_start, int rank_start, char file_end, int rank_end, char promote) const
{
    std::vector<const Move*> move_list;
    for(const auto& move : legal_moves())
    {
        if(move->start_file() == file_start &&
           move->start_rank() == rank_start &&
           move->end_file() == file_end &&
           move->end_rank() == rank_end)
        {
            if(promote)
            {
                if(move->promotion_piece_symbol() == std::toupper(promote))
                {
                    return *move;
                }
            }
            else
            {
                move_list.push_back(move);
            }
        }
    }

    if(move_list.empty())
    {
        throw Illegal_Move("No legal move found for " +
                           std::string(1, file_start) +
                           std::to_string(rank_start) +
                           "-" +
                           std::string(1, file_end) +
                           std::to_string(rank_end) +
                           " (" + std::string(1, promote ? promote : ' ') + ")");
    }

    if(move_list.size() == 1)
    {
        return *move_list.front();
    }
    else
    {
        throw Promotion_Piece_Needed();
    }
}

//! Updates the state of the board according to a Player-selected Move.

//! \param move A Move-class instance. This must be an item taken from Board::legal_moves().
//!        The input move is only checked for legality in DEBUG builds, where illegal moves
//!        will trip an assert. In RELEASE builds, submitting an illegal move is undefined,
//!        resulting in anything from an invalid board state to a crash due to segfault.
//! \returns Returns a Game_Result indicating the result of the move and whether the game has ended.
Game_Result Board::submit_move(const Move& move)
{
    update_board(move);

    if(no_legal_moves())
    {
        if(king_is_in_check())
        {
            auto winner = opposite(whose_turn());
            return Game_Result(winner, color_text(winner) + " mates");
        }
        else
        {
            return Game_Result(NONE, "Stalemate");
        }
    }

    // An insufficient material draw can only happen after a capture
    // or a pawn promotion to a minor piece, both of which clear the
    // repeat_count tracker.
    if(moves_since_pawn_or_capture() == 0 && ! enough_material_to_checkmate())
    {
        return Game_Result(NONE, "Insufficient material");
    }

    if(current_board_position_repeat_count() >= 3)
    {
        return Game_Result(NONE, "Threefold repetition");
    }

    // "Move" means both players move, so the fifty-move rule is
    // triggered after 100 player moves
    if(moves_since_pawn_or_capture() >= 100)
    {
        return Game_Result(NONE, "50-move limit");
    }

    return {};
}

void Board::update_board(const Move& move)
{
    assert(is_in_legal_moves_list(move));
    game_record_listing.push_back(&move);

    make_move(move.start_file(), move.start_rank(),
              move.end_file(), move.end_rank());
    move.side_effects(*this);

    switch_turn();

    add_board_position_to_repeat_record();
}

//! Creates a Move instance given a text string representation.

//! \param move A string using coordinate notation ("a2b3") or PGN ("Bb3").
//! \returns A Move instance corresponding to the input string.
//! \throws Illegal_Move if the text does not represent a legal move or if the wanted move is ambiguous.
const Move& Board::create_move(const std::string& move) const
{
    const std::string pieces = "RNBQK";
    const std::string valid_files = "abcdefgh";
    const std::string valid_rows = "12345678";
    const std::string castling = "O";
    const std::string valid_characters = pieces + valid_files + valid_rows + castling;

    std::string validated;
    char promoted_piece = 0;
    if(String::contains(move, '='))
    {
        for(size_t i = move.find('=') + 1; i < move.size(); ++i)
        {
            if(String::contains(pieces, move[i]) && move[i] != 'K')
            {
                promoted_piece = move[i];
                break;
            }
        }
    }

    for(char c : move)
    {
        if(c == '=')
        {
            break;
        }

        if(String::contains(valid_characters, c))
        {
            validated.push_back(c);
        }
    }

    if(validated.size() < 2)
    {
        throw Illegal_Move(move + " does not specify a valid move.");
    }

    // Castling
    if(validated == "OO")
    {
        return create_move('e', whose_turn() == WHITE ? 1 : 8,
                           'g', whose_turn() == WHITE ? 1 : 8);
    }
    if(validated == "OOO")
    {
        return create_move('e', whose_turn() == WHITE ? 1 : 8,
                           'c', whose_turn() == WHITE ? 1 : 8);
    }

    // Normal PGN move
    std::string piece_symbol = (String::contains(pieces, validated[0]) ? validated.substr(0, 1) : "");

    char starting_file = 0;
    int  starting_rank = 0;

    if(validated.size() == 5 && ! piece_symbol.empty()) // Bb2c3
    {
        starting_file = validated[1];
        starting_rank = validated[2] - '0';
    }
    else if(validated.size() == 4 && ! piece_symbol.empty()) // Raa5 or R5c5
    {
        if(std::isdigit(validated[1]))
        {
            starting_rank = validated[1] - '0';
        }
        else
        {
            starting_file = validated[1];
        }
    }
    else if(validated.size() <= 3 && piece_symbol.empty()) // Pawn move/capture de5 (dxe5)
    {
        starting_file = validated.front();
    }
    else if(piece_symbol.empty())
    {
        // No PGN-style move works, try coordinate move (e.g., e7e8q)
        if(move.size() < 4 || move.size() > 5)
        {
            throw Illegal_Move("Illegal text move (wrong length): " + move);
        }

        char start_file = move[0];
        int  start_rank = move[1] - '0';
        char end_file   = move[2];
        int  end_rank   = move[3] - '0';
        if(move.size() == 5)
        {
            promoted_piece = std::toupper(move[4]);
        }

        return create_move(start_file, start_rank, end_file, end_rank, promoted_piece);
    }

    // else normal PGN-style piece movement
    char ending_file = validated[validated.size() - 2];
    int  ending_rank = validated[validated.size() - 1] - '0';
    if( ! inside_board(ending_file, ending_rank))
    {
        throw Illegal_Move("Illegal text move (out of board): " + move);
    }

    char file_search_start = (starting_file == 0 ? 'a' : starting_file);
    char file_search_end   = (starting_file == 0 ? 'h' : starting_file);

    int rank_search_start = (starting_rank == 0 ? 1 : starting_rank);
    int rank_search_end   = (starting_rank == 0 ? 8 : starting_rank);

    if(starting_file == 0 || starting_rank == 0)
    {
        for(char file = file_search_start; file <= file_search_end; ++file)
        {
            for(int rank = rank_search_start; rank <= rank_search_end; ++rank)
            {
                auto piece = piece_on_square(file, rank);
                if(piece &&
                   piece->pgn_symbol() == piece_symbol &&
                   is_legal(file, rank, ending_file, ending_rank))
                {
                    if(starting_file == 0 || starting_rank == 0)
                    {
                        starting_file = file;
                        starting_rank = rank;
                    }
                    else
                    {
                        // If two moves satisfy text, argument is ambiguous.
                        throw Illegal_Move("Ambiguous move: " + move);
                    }
                }
            }
        }
    }

    if(starting_file != 0 && starting_rank != 0)
    {
        return create_move(starting_file, starting_rank,
                           ending_file,   ending_rank,
                           promoted_piece);
    }

    throw Illegal_Move("Malformed move: " + move);
}

void Board::make_move(char file_start, int rank_start, char file_end, int rank_end)
{
    if(piece_on_square(file_end, rank_end))
    {
        clear_repeat_count();
    }

    place_piece(piece_on_square(file_start, rank_start), file_end, rank_end);
    remove_piece(file_start, rank_start);

    clear_en_passant_target();
    clear_pinned_squares();
}

//! Tells which player is due to move.

//! \returns Color of player who is next to move.
Color Board::whose_turn() const
{
    return turn_color;
}

//! Get a list of all legal moves for the current player.

//! \returns A list of pointers to legal moves. Any call to Board::submit_move() must take
//!          its argument from this list.
const std::vector<const Move*>& Board::legal_moves() const
{
    return legal_moves_cache;
}

//! Prints an ASCII version of the board to a terminal.

//! This is useful for Human_Player on the terminal and debugging.
//! \param perspective Specifies which side of the board is at the bottom of the screen.
void Board::ascii_draw(Color perspective) const
{
    const size_t square_width = 7;
    const size_t square_height = 3;

    const std::string square_corner = "+";
    const std::string square_horizontal_border = "-";
    const std::string square_vertical_border = "|";

    std::string horizontal_line;
    for(int i = 0; i < 8; ++i)
    {
        horizontal_line.append(square_corner);

        for(size_t j = 0; j < square_width; ++j)
        {
            horizontal_line.append(square_horizontal_border);
        }
    }
    horizontal_line.append(square_corner + "\n");
    const std::string left_spacer = "   ";

    int rank_start = (perspective == WHITE ? 8 : 1);
    int rank_end = (perspective == WHITE ? 1 : 8);
    int d_rank = (perspective == WHITE ? -1 : 1);
    char file_start = (perspective == WHITE ? 'a' : 'h');
    char file_end = (perspective == WHITE ? 'h' : 'a');
    int d_file = (perspective == WHITE ? 1 : -1);

    for(int rank = rank_start; d_rank*(rank_end - rank) >= 0; rank += d_rank)
    {
        std::cout << left_spacer << horizontal_line;

        for(size_t square_row = 0; square_row < square_height; ++square_row)
        {
            if(square_row == square_height/2)
            {
                std::cout << " " << rank << " ";
            }
            else
            {
                std::cout << left_spacer;
            }
            for(char file = file_start; d_file*(file_end - file) >= 0; file += d_file)
            {
                std::string piece_symbol;
                auto piece = piece_on_square(file, rank);
                char filler = ' ';
                if(piece)
                {
                    auto piece_row = piece->ascii_art(square_row);
                    std::string padding((square_width - piece_row.size())/2, filler);
                    piece_symbol = padding + piece_row + padding;
                    while(piece_symbol.size() < square_width)
                    {
                        piece_symbol.push_back(filler);
                    }
                }
                else
                {
                    piece_symbol.append(square_width, filler);
                }
                if(square_color(file, rank) == BLACK)
                {
                    for(auto start : {0, int(piece_symbol.size() - 1)})
                    {
                        auto direction = (start == 0 ? 1 : -1);
                        for(int i = start; i >= 0 && i < int(piece_symbol.size()); i += direction)
                        {
                            if(piece_symbol[i] == filler)
                            {
                                char dark_square_fill = ':';
                                piece_symbol[i] = dark_square_fill;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
                std::cout << square_vertical_border << piece_symbol;
            }
            std::cout << square_vertical_border << "\n";
        }
    }
    std::cout << left_spacer << horizontal_line;

    std::cout << left_spacer;
    for(char file = file_start; d_file*(file_end - file) >= 0; file += d_file)
    {
        std::cout << " " << std::string(square_width/2, ' ')
                  << file
                  << std::string(square_width/2, ' ');
    }
    std::cout << std::endl << std::endl;
}

void Board::remove_piece(char file, int rank)
{
    place_piece(nullptr, file, rank);
}

void Board::place_piece(const Piece* piece, char file, int rank)
{
    update_board_hash(file, rank); // XOR out piece on square

    piece_on_square(file, rank) = piece;
    unmoved_positions[square_index(file, rank)] = false;

    update_board_hash(file, rank); // XOR in new piece on square

    if(piece && piece->type() == KING)
    {
        king_location[piece->color()] = {file, rank};
    }
}

//! Find out if the king of the player to move is currently in check.

//! \returns If the current player is in check.
bool Board::king_is_in_check() const
{
    return checking_squares.front(); // check whether first entry is valid Square
}

//! Find out whether the input square is safe for the given king to occupy.

//! A square is not safe for the king if the opposing pieces can attack the square.
//! \param file File of the queried square.
//! \param rank Rank of the queried square.
//! \param king_color The color of the king piece that is under potential attack.
//! \returns Whether the king would be in check if it was placed on the square in question.
bool Board::safe_for_king(char file, int rank, Color king_color) const
{
    return Threat_Generator(file, rank, opposite(king_color), *this).empty();
}

//! List of all squares and whether they are attacked by currently legal moves.

//! \returns An array of bools with indices given by Board::square_index(). A value of true indicates that the square
//!          is under attack by the player about to move.
const std::array<bool, 64>& Board::all_square_indices_attacked() const
{
    return attacked_indices;
}

//! List of all squares and whether they are attacked by currently illegal moves.

//! \returns An array of bools with indices given by Board::square_index(). A value of true indicates that the square
//!          would be under attack by the player about to move except for the move being blocked, the piece being pinned,
//!          or the current player's king being in check.
const std::array<bool, 64>& Board::other_square_indices_attacked() const
{
    return other_attacked_indices;
}

//! List of squares and whether they are safe for the king of the player not to move to occupy.

//! \returns An array of bools with indices given by Board::square_index(). A value of true indicates
//!          that the king of the player not moving would be free of check if placed there.
const std::array<bool, 64>& Board::squares_safe_for_king() const
{
    return safe_squares_for_king;
}

void Board::refresh_checking_squares()
{
    checking_squares = {};
    size_t insertion_point = 0;
    const auto& king_square = find_king(whose_turn());

    if(game_record_listing.empty())
    {
        for(auto square : Threat_Generator(king_square.file(), king_square.rank(), opposite(whose_turn()), *this))
        {
            checking_squares[insertion_point++] = square;
        }
    }
    else
    {
        auto last_move = game_record_listing.back();

        // Moved piece now attacks king
        if(attacks(last_move->end_file(), last_move->end_rank(), king_square.file(), king_square.rank()))
        {
            checking_squares[insertion_point++] = {last_move->end_file(), last_move->end_rank()};
        }

        // Discovered check
        if(auto pinning_square = piece_is_pinned(last_move->start_file(), last_move->start_rank()))
        {
            // Prevent pawn promotions from registering twice, for example
            // ...   a8=Q     Q..
            // P.. -------->  ... Check from queen on a8 and discovered check by queen on a8
            // k..            k..
            if(pinning_square != checking_squares.front())
            {
                checking_squares[insertion_point++] = pinning_square;
            }
        }

        // Discovered check due to en passant
        if(last_move->is_en_passant())
        {
            if(auto pinning_square = piece_is_pinned(last_move->end_file(), last_move->start_rank()))
            {
                // Since two pieces are removed, make sure the discovered check isn't recorded twice
                if(pinning_square != checking_squares.front())
                {
                    checking_squares[insertion_point++] = pinning_square;
                }
            }
        }

        // Discovered check by rook due to castling
        if(last_move->is_castling())
        {
            char rook_file = (last_move->file_change() > 0 ? 'f' : 'd');

            // If the non-castling king is on the same rank as the castling king, the check will have
            // been found by the discovered check block above. Only look for checks along columns.
            if(king_square.file() == rook_file && all_empty_between(rook_file, last_move->end_rank(), king_square.file(), king_square.rank()))
            {
                checking_squares[insertion_point] = {rook_file, last_move->end_rank()};
            }
        }
    }
}

//! Check if a move will leave the player making the move in check.

//! \param move A possibly legal move to check.
//! \returns Whether the move under consideration will leave the friendly king in check.
bool Board::king_is_in_check_after_move(const Move& move) const
{
    if(piece_on_square(move.start_file(), move.start_rank())->type() == KING)
    {
        return ! safe_for_king(move.end_file(), move.end_rank(), whose_turn());
    }

    if(king_is_in_check())
    {
        // King is under check from multiple pieces but not moving
        if(king_multiply_checked())
        {
            return true; // moving a piece can only block one checking piece
        }

        // Checking piece is captured by non-pinned piece
        if(checking_squares.front() == Square{move.end_file(), move.end_rank()})
        {
            return piece_is_pinned(move.start_file(), move.start_rank());
        }

        // Non-pinned piece moves to block check
        if( ! piece_on_square(move.end_file(), move.end_rank()))
        {
            if(piece_is_pinned(move.end_file(), move.end_rank()))
            {
                return piece_is_pinned(move.start_file(), move.start_rank());
            }
        }

        // En passant capture of checking pawn
        if(move.is_en_passant())
        {
            return (checking_squares.front() != Square{move.end_file(), move.start_rank()}) || piece_is_pinned(move.start_file(), move.start_rank());
        }

        // Nothing is done about the check
        return true;
    }

    if(auto pinning_square = piece_is_pinned(move.start_file(), move.start_rank()))
    {
        const auto& king_square = find_king(whose_turn());

        auto pin_direction_file = king_square.file() - pinning_square.file();
        auto pin_direction_rank = king_square.rank() - pinning_square.rank();

        // If move direction and pin direction are not parallel, the piece will move
        // out of pin and expose the king to check.
        return ! moves_are_parallel(move.file_change(), move.rank_change(), pin_direction_file, pin_direction_rank);
    }

    if(move.is_en_passant())
    {
        // Check if king is on same row as both pawns and there is an opposing rook or queen on row
        const auto& king_square = find_king(whose_turn());
        if(king_square.rank() != move.start_rank())
        {
            return false;
        }

        auto step = (move.start_file() > king_square.file() ? 1 : -1);
        auto attacking_file = move.start_file();
        auto attacked_file = move.end_file();
        auto passed_pawns = 0;
        for(char file = king_square.file() + step; inside_board(file); file += step)
        {
            if(file == attacked_file || file == attacking_file)
            {
                ++passed_pawns;
                continue;
            }

            auto piece = piece_on_square(file, king_square.rank());
            if(piece)
            {
                if(passed_pawns < 2)
                {
                    return false;
                }

                if(piece->color() == whose_turn())
                {
                    return false;
                }

                return piece->type() == ROOK || piece->type() == QUEEN;
            }
        }
    }

    return false;
}

bool Board::no_legal_moves() const
{
    return legal_moves().empty();
}

//! Get the history of moves.

//! \returns The list of moves made on this board.
const std::vector<const Move*>& Board::game_record() const
{
    return game_record_listing;
}

template<typename OutputStream, typename DataType>
void print_game_header_line(OutputStream& output, const std::string& heading, const DataType& data)
{
    output << "[" << heading << " \"" << data << "\"]\n";
}

//! Prints the PGN game record with commentary from Players.

//! \param white Pointer to Player playing white to provide commentary for moves. Can be nullptr.
//! \param black Pointer to Player playing black to provide commentary for moves. Can be nullptr.
//! \param file_name Name of the text file where the game will be printed. If empty, print to stdout.
//! \param result The result of the last action (move, clock punch, or outside intervention) in the game.
//! \param game_clock The game clock used during the game.
void Board::print_game_record(const Player* white,
                              const Player* black,
                              const std::string& file_name,
                              const Game_Result& result,
                              const Clock& game_clock) const
{
    static std::mutex write_lock;
    std::lock_guard<std::mutex> write_lock_guard(write_lock);

    static int game_number = 0;
    if(game_number == 0)
    {
        game_number = 1;
        std::ifstream ifs(file_name);
        std::string line;
        while(std::getline(ifs, line))
        {
            if(String::starts_with(line, "[Round"))
            {
                auto round_number = std::stoi(String::split(line, "\"").at(1));
                if(round_number >= game_number)
                {
                    game_number = round_number + 1;
                }
            }
        }
    }

    std::ofstream ofs(file_name, std::ios::app);
    std::ostream& out_stream = (ofs ? ofs : std::cout);

    auto game_start_time = game_clock.game_start_date_and_time();
    auto game_start_time_c = std::chrono::system_clock::to_time_t(game_start_time);
    std::tm time_out;
#ifdef _WIN32
    localtime_s(&time_out, &game_start_time_c);
#elif defined(__linux__)
    localtime_r(&game_start_time_c, &time_out);
#endif
    print_game_header_line(out_stream, "Date", std::put_time(&time_out, "%Y.%m.%d"));
    print_game_header_line(out_stream, "Time", std::put_time(&time_out, "%H:%M:%S"));

    if(white && ! white->name().empty())
    {
        print_game_header_line(out_stream, "White", white->name());
    }

    if(black && ! black->name().empty())
    {
        print_game_header_line(out_stream, "Black", black->name());
    }

    print_game_header_line(out_stream, "Round", game_number++);

    if(game_clock.initial_time() > 0)
    {
        std::ostringstream time_control_spec;
        if(game_clock.moves_to_reset() > 0)
        {
            time_control_spec << game_clock.moves_to_reset() << '/';
        }
        time_control_spec << game_clock.initial_time();
        if(game_clock.increment() > 0)
        {
            time_control_spec << '+' << game_clock.increment();
        }
        print_game_header_line(out_stream, "TimeControl", time_control_spec.str());
        print_game_header_line(out_stream, "TimeLeftWhite", game_clock.time_left(WHITE));
        print_game_header_line(out_stream, "TimeLeftBlack", game_clock.time_left(BLACK));
    }

    if( ! result.game_ending_annotation().empty())
    {
        print_game_header_line(out_stream, "Result", result.game_ending_annotation());
    }

    if( ! result.ending_reason().empty() && ! String::contains(result.ending_reason(), "mates"))
    {
        print_game_header_line(out_stream, "Termination", result.ending_reason());
    }

    auto temp = Board(starting_fen);
    if(starting_fen != standard_starting_fen)
    {
        print_game_header_line(out_stream, "SetUp", 1);
        print_game_header_line(out_stream, "FEN", starting_fen);
    }

    auto starting_turn_offset = (temp.whose_turn() == WHITE ? 0 : 1);

    for(size_t i = 0; i < game_record_listing.size(); ++i)
    {
        if(temp.whose_turn() == WHITE || i == 0)
        {
            auto step = (i + starting_turn_offset)/2;
            out_stream << '\n' << move_count_start_offset + step << ".";
            if(i == 0 && temp.whose_turn() == BLACK)
            {
                out_stream << " ...";
            }
        }

        auto next_move = game_record_listing.at(i);
        out_stream << " " << next_move->game_record_item(temp);

        auto current_player = (temp.whose_turn() == WHITE ? white : black);
        if(current_player)
        {
            auto commentary = String::trim_outer_whitespace(current_player->commentary_for_next_move(temp));
            if( ! commentary.empty())
            {
                out_stream << " { " << commentary << " }";
            }
        }

        temp.submit_move(*next_move);
    }
    out_stream << "\n\n\n";
}

//! Prints the last move of the game in coordinate notation.

//! Used to communicate with external chess programs. This is an altnative to
//! Board::last_move_record() that doesn't need to replay the game to create
//! the PGN notation move.
//! \returns Coordinate-style move string ("e2e4").
std::string Board::last_move_coordinates() const
{
    return game_record_listing.back()->coordinate_move();
}

//! Change whose turn it is to move.

//! If the input color is not the current player to move, this is
//! equivalent to skipping the current player's turn. Otherwise,
//! there's no effect.
//! \param color The color of the player who is next to move.
void Board::set_turn(Color color)
{
    if(turn_color != color)
    {
        clear_en_passant_target();
        switch_turn();
    }
}

void Board::switch_turn()
{
    turn_color = opposite(turn_color);
    update_whose_turn_hash();
    recreate_move_caches();
}

void Board::make_en_passant_targetable(char file, int rank)
{
    if(en_passant_target)
    {
        update_board_hash(en_passant_target.file(),
                          en_passant_target.rank()); // XOR out previous en passant target
    }

    en_passant_target = {file, rank};
    update_board_hash(file, rank);
}

//! Determine whether the indicated square can be a target of an en passant move.

//! The is called by En_Passant::move_specific_legal().
//! \param file File of the queried square.
//! \param rank Rank of the queried square.
//! \returns Whether the square was passed by a pawn double move.
bool Board::is_en_passant_targetable(char file, int rank) const
{
    return en_passant_target.file() == file &&
           en_passant_target.rank() == rank;
}

void Board::clear_en_passant_target()
{
    make_en_passant_targetable('\0', 0);
}

void Board::clear_pinned_squares()
{
    square_searched_for_pin.fill(false);
}

//! Indicates whether the queried square has a piece on it that never moved.

//! \param file File of queried square.
//! \param rank Rank of queried squaer.
//! \returns If the piece on the square has never moved during the game.
bool Board::piece_has_moved(char file, int rank) const
{
    return ! unmoved_positions[square_index(file, rank)];
}

//! Finds the square on which a king resides.

//! \param color Which king to find.
//! \returns Square which contains the sought after king.
const Square& Board::find_king(Color color) const
{
    assert(piece_on_square(king_location[color].file(), king_location[color].rank()) == piece_instance(KING, color));
    return king_location[color];
}

void Board::recreate_move_caches()
{
    legal_moves_cache.clear();
    refresh_checking_squares();
    attacked_indices.fill(false);
    other_attacked_indices.fill(false);
    safe_squares_for_king.fill(true);

    bool en_passant_legal = false;
    material_change_move_available = false;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = piece_on_square(file, rank);
            if(piece && piece->color() == whose_turn())
            {
                // invalid direction
                const Move* blocked_move = nullptr;

                for(const auto move : piece->move_list(file, rank))
                {
                    auto blocked = blocked_move &&
                                   same_direction(move->file_change(),         move->rank_change(),
                                                  blocked_move->file_change(), blocked_move->rank_change());
                    auto legal = false;

                    if( ! blocked)
                    {
                        blocked_move = nullptr;
                        if(move->is_legal(*this))
                        {
                            legal = true;
                            legal_moves_cache.push_back(move);

                            if(move->can_capture())
                            {
                                auto rank_adjust = 0;
                                if(move->is_en_passant())
                                {
                                    rank_adjust = -move->rank_change();
                                    en_passant_legal = true;
                                    material_change_move_available = true;
                                }
                                attacked_indices[square_index(move->end_file(),
                                                              move->end_rank() + rank_adjust)] = true;
                            }

                            material_change_move_available = material_change_move_available ||
                                                             move_captures(*move)||
                                                             move->promotion_piece_symbol();
                        }

                        // If there is a piece on the ending square, farther moves are not possible.
                        // The check for a promotion piece is needed since the set of pawn promotions
                        // results in moves with identical beginning and ending squares, which would
                        // result in the first pawn-promotion-by-capture blocking all the others.
                        if(piece_on_square(move->end_file(), move->end_rank()) && ! move->promotion_piece_symbol())
                        {
                            blocked_move = move;
                        }

                        auto& safe = safe_squares_for_king[square_index(move->end_file(), move->end_rank())];
                        if(safe)
                        {
                            safe = ! move->can_capture();
                        }
                    }

                    if( ! legal && move->can_capture())
                    {
                        other_attacked_indices[square_index(move->end_file(), move->end_rank())] = true;
                    }
                }
            }
        }
    }

    // Make sure board hash is correct for valid repeat counts
    if( ! en_passant_legal)
    {
        clear_en_passant_target();
    }
}

//! Checks whether there are enough pieces on the board for any possible checkmate.

//! The following piece sets on the board make checkmate possible:
//! - Any single pawn, rook, or queen,
//! - At least two bishops (of any color) on oppositely colored squares,
//! - A bishop and knight (of an color combination),
//! - Two knights (of any color combination).
//! \param color If NONE, check both sides for enough material. Otherwise, only check the pieces of one side.
//! \returns If there are enough pieces on the board to make a checkmate arrangement.
//!          If the method returns false when called with NONE, this will usually lead to a drawn game.
bool Board::enough_material_to_checkmate(Color color) const
{
    bool knight_found = false;
    Color bishop_square_color_found = NONE;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = piece_on_square(file, rank);
            if( ! piece)
            {
                continue;
            }

            if(color != NONE && piece->color() != color)
            {
                continue;
            }

            auto type = piece->type();
            if(type == KING)
            {
                continue;
            }

            if(type == QUEEN || type == ROOK || type == PAWN)
            {
                return true; // checkmate possible with just queen or rook; pawn can promote
            }

            // Newly found piece is either a bishop or knight
            if(knight_found)
            {
                return true; // checkmate with a knight and any non-king piece on either side is possible
            }

            if(type == KNIGHT)
            {
                if(bishop_square_color_found != NONE)
                {
                    return true;
                }
                knight_found = true;
            }
            else // BISHOP
            {
                auto bishop_square_color = square_color(file, rank);
                if(bishop_square_color_found == NONE)
                {
                    bishop_square_color_found = bishop_square_color;
                }
                else if(bishop_square_color != bishop_square_color_found)
                {
                    return true; // checkmate with opposite colored bishops possible
                }
            }
        }
    }

    return false;
}

//! Get last move for dispaly in text-based UIs.

//! \returns Last move on this board in PGN notation
std::string Board::last_move_record() const
{
    Board b(starting_fen);
    std::string result;
    for(const auto& move : game_record())
    {
        result = move->game_record_item(b);
        b.submit_move(*move);
    }

    return result;
}

//! Set the format an engine should output while picking a move.

//! \param mode Which chess engine protocol is being used: CECP or NO_THINKING.
void Board::set_thinking_mode(Thinking_Output_Type mode) const
{
    thinking_indicator = mode;
}

//! Find out what kind of format an engine should output while picking a move.

//! \returns Format of thinking output: CECP or NO_THINKING.
Thinking_Output_Type Board::thinking_mode() const
{
    return thinking_indicator;
}

void Board::initialize_board_hash()
{
    static std::mutex hash_lock;
    std::lock_guard<std::mutex> hash_guard(hash_lock);

    static bool hash_values_initialized = false;
    if(hash_values_initialized)
    {
        return;
    }

    switch_turn_board_hash = Random::random_unsigned_int64();

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto index = square_index(file, rank);
            en_passant_hash_values[index] = Random::random_unsigned_int64();
            castling_hash_values[index] = Random::random_unsigned_int64();

            for(auto piece_color : {BLACK, WHITE})
            {
                for(auto piece_type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
                {
                    auto hash_index = square_hash_index(piece_instance(piece_type, piece_color));
                    square_hash_values[index][hash_index] = Random::random_unsigned_int64();
                }
            }

            square_hash_values[index][square_hash_index(nullptr)] = Random::random_unsigned_int64();
        }
    }

    current_board_hash = 0;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            current_board_hash ^= square_hash(file, rank);
        }
    }

    hash_values_initialized = true;
}

void Board::update_board_hash(char file, int rank)
{
    current_board_hash ^= square_hash(file, rank);
}

void Board::update_whose_turn_hash()
{
    current_board_hash ^= switch_turn_board_hash;
}

uint64_t Board::square_hash(char file, int rank) const
{
    if( ! Board::inside_board(file, rank))
    {
        return 0; // do nothing for squares outside of board (e.g., clear_en_passant_target()).
    }

    auto piece = piece_on_square(file, rank);
    auto index = square_index(file, rank);
    auto result = square_hash_values[index][square_hash_index(piece)];
    if(piece &&
       piece->type() == ROOK &&
       ! piece_has_moved(file, rank) &&
       ! piece_has_moved('e', rank))
    {
        result ^= castling_hash_values[index];
    }

    if( ! piece && is_en_passant_targetable(file, rank))
    {
        result = en_passant_hash_values[index];
    }

    return result;
}

size_t Board::square_hash_index(const Piece* piece)
{
    if(piece)
    {
        return piece->type() + (piece->color()*6); // 6 == number of piece types
    }
    else
    {
        return square_hash_values.front().size() - 1; // last value for empty square
    }
}

uint64_t Board::board_hash() const
{
    return current_board_hash;
}

//! Report if the last move change the pieces on the board.

//! This is more general than Board::last_move_captured() since it also checks
//! for pawn promotions.
//! \returns Whether the last move captured or resulted in a promoted pawn.
bool Board::last_move_changed_material() const
{
    return last_move_captured() ||
           ( ! game_record_listing.empty() && game_record_listing.back()->promotion_piece_symbol());
}

//! Report if the most recent move in the game captured a piece.

//! This method is used by chess engines to decide how interesting a board position is
//! and how much time to devote to future moves.
//! \returns Whether the last move captured an opponent's piece.
bool Board::last_move_captured() const
{
    if(game_record_listing.empty() || moves_since_pawn_or_capture() > 0)
    {
        return false;
    }

    auto last_move = game_record_listing.back();
    if(piece_on_square(last_move->end_file(), last_move->end_rank())->type() == PAWN ||
       last_move->promotion_piece_symbol())
    {
        // Check if move was pawn capture
        return last_move->file_change() != 0;
    }

    // Not a pawn move and moves_since_pawn_or_capture() == 0 ==> capturing move
    return true;
}

//! Check if any currently legal move changes material by capturing or promoting a pawn.

//! This method is used by chess engines to determine how interesting a board position
//! is and thus how much time to devote to studying future moves.
//! \returns If any legal move captures or promotes a pawn.
bool Board::material_change_possible() const
{
    return material_change_move_available;
}

//! Determines whether a move captures on the current board.

//! \param move Move to check.
//! \returns If the move captures an opponent's piece.
//! \throws assertion_failure In DEBUG builds, if the move to check is not legal, an assert fails.
bool Board::move_captures(const Move& move) const
{
    auto attacked_piece = piece_on_square(move.end_file(), move.end_rank());

    // Assert move is actually legal
    assert(is_in_legal_moves_list(move));
    assert( ! attacked_piece || (move.can_capture() && attacked_piece->color() == opposite(whose_turn())));

    return attacked_piece || move.is_en_passant();
}

bool Board::king_multiply_checked() const
{
    return checking_squares.back(); // See if second entry is valid Square
}

//! Check whether all of the squares between two squares are empty.

//! This method assumes the squares are along the same rank, file, or diagonal.
//! \param file_start File of the first square.
//! \param rank_start Rank of the first square.
//! \param file_end File of the second square.
//! \param rank_end Rank of the second square.
//! \returns Whether all of the intervening squares between the two input squares are unoccupied by pieces.
//! \throws assertion_failure In DEBUG builds, squares not along the same rank, file, or diagonal
//!         will trigger an assertion failure. In RELEASE builds, the result is undefined.
bool Board::all_empty_between(char file_start, int rank_start, char file_end, int rank_end) const
{
    assert(straight_line_move(file_start, rank_start, file_end, rank_end));

    auto file_step = Math::sign(file_end - file_start);
    auto rank_step = Math::sign(rank_end - rank_start);

    char file = file_start + file_step;
    int  rank = rank_start + rank_step;

    while(file != file_end || rank != rank_end)
    {
        assert(inside_board(file, rank));

        if(piece_on_square(file, rank))
        {
            return false;
        }

        file += file_step;
        rank += rank_step;
    }

    return true;
}

bool Board::straight_line_move(char file_start, int rank_start, char file_end, int rank_end)
{
    auto file_change = file_end - file_start;
    if(file_change == 0)
    {
        return true;
    }

    auto rank_change = rank_end - rank_start;
    if(rank_change == 0)
    {
        return true;
    }

    return std::abs(file_change) == std::abs(rank_change);
}

bool Board::moves_are_parallel(int file_change_1, int rank_change_1, int file_change_2, int rank_change_2)
{
    // Think of the determinant of a 2x2 matrix with the two moves as column vectors
    return file_change_1*rank_change_2 == file_change_2*rank_change_1;
}

bool Board::same_direction(int file_change_1, int rank_change_1, int file_change_2, int rank_change_2)
{
    return moves_are_parallel(file_change_1, rank_change_1, file_change_2, rank_change_2) &&
           file_change_1*file_change_2 + rank_change_1*rank_change_2 > 0; // dot product
}

bool Board::attacks(char origin_file, int origin_rank, char target_file, int target_rank) const
{
    auto attacking_piece = piece_on_square(origin_file, origin_rank);
    if( ! attacking_piece)
    {
        return false;
    }

    auto file_change = std::abs(target_file - origin_file);
    auto rank_change = std::abs(target_rank - origin_rank);

    if(attacking_piece->type() == KNIGHT)
    {
        return file_change*rank_change == 2; // 1x2 or 2x1 move
    }

    if( ! straight_line_move(origin_file, origin_rank,
                             target_file, target_rank))
    {
        return false;
    }

    if(attacking_piece->type() == KING)
    {
        return std::max(file_change, rank_change) == 1;
    }

    if(attacking_piece->type() == PAWN)
    {
        return file_change*rank_change == 1 && // 1x1 move
               (attacking_piece->color() == WHITE) == (target_rank > origin_rank);
    }

    if( ! all_empty_between(origin_file, origin_rank, target_file, target_rank))
    {
        return false;
    }

    if(attacking_piece->type() == QUEEN)
    {
        return true;
    }

    if(file_change == rank_change)
    {
        return attacking_piece->type() == BISHOP;
    }
    else
    {
        return attacking_piece->type() == ROOK;
    }
}

//! Determine whether a piece would be pinned to the king by an opposing piece if it was on the given square.

//! \param file File of queried square.
//! \param rank Rank of queried square.
//! \returns The Square upon which an opposing piece stands that would pin a friendly
//!          piece to the king. If no such Square exists, then an invalid square
//!          is returned (which converts to bool(false) in if() statements).
Square Board::piece_is_pinned(char file, int rank) const
{
    auto index = square_index(file, rank);
    if(square_searched_for_pin[index])
    {
        return pinning_squares[index];
    }

    square_searched_for_pin[index] = true;
    auto& last_found_pinning_square = pinning_squares[index];

    const auto& king_square = find_king(whose_turn());
    const static auto no_pin = Square{};

    if(king_square == Square{file, rank})
    {
        return last_found_pinning_square = no_pin; // king is never pinned
    }

    if( ! straight_line_move(file, rank, king_square.file(), king_square.rank()))
    {
        return last_found_pinning_square = no_pin;
    }

    if( ! all_empty_between(king_square.file(), king_square.rank(), file, rank))
    {
        return last_found_pinning_square = no_pin;
    }

    auto file_step = Math::sign(file - king_square.file());
    auto rank_step = Math::sign(rank - king_square.rank());

    for(int step = 1; true; ++step) // loop until outside board or piece found
    {
        char current_file = file + file_step*step;
        int  current_rank = rank + rank_step*step;

        if( ! inside_board(current_file, current_rank))
        {
            return last_found_pinning_square = no_pin;
        }

        auto attacking_piece = piece_on_square(current_file, current_rank);
        if(attacking_piece)
        {
            if(attacking_piece->color() == whose_turn())
            {
                return last_found_pinning_square = no_pin;
            }

            if(attacking_piece->type() == QUEEN)
            {
                return last_found_pinning_square = {current_file, current_rank};
            }

            if(file_step == 0 || rank_step == 0)
            {
                if(attacking_piece->type() == ROOK)
                {
                    return last_found_pinning_square = {current_file, current_rank};
                }
            }
            else
            {
                if(attacking_piece->type() == BISHOP)
                {
                    return last_found_pinning_square = {current_file, current_rank};
                }
            }

            return last_found_pinning_square = no_pin;
        }
    }
}

void Board::add_board_position_to_repeat_record()
{
    add_to_repeat_count(board_hash());
}

void Board::add_to_repeat_count(uint64_t new_hash)
{
    repeat_count[repeat_count_insertion_point++] = new_hash;
}

size_t Board::current_board_position_repeat_count() const
{
    return std::count(repeat_count.begin(),
                      repeat_count.begin() + repeat_count_insertion_point,
                      board_hash());
}

//! The number of moves since the last capture or pawn move.

//! \returns How many moves have been made since the last capturing or pawn move.
int Board::moves_since_pawn_or_capture() const
{
    return repeat_count_insertion_point - 1;
}

void Board::clear_repeat_count()
{
    repeat_count_insertion_point = 0;
}

//! Gets the ply move during which a player castled.

//! \param player The color of the player being queried.
//! \returns The ply count when the player castled, or MAX(size_t) if the player has not castled.
size_t Board::castling_move_index(Color player) const
{
    return castling_index[player];
}
