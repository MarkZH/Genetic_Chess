#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Clock.h"

#include "Pieces/Pawn.h"
#include "Pieces/Rook.h"
#include "Pieces/Knight.h"
#include "Pieces/Bishop.h"
#include "Pieces/King.h"
#include "Pieces/Queen.h"

#include "Moves/Move.h"

#include "Exceptions/Illegal_Move_Exception.h"
#include "Exceptions/Checkmate_Exception.h"
#include "Exceptions/Stalemate_Exception.h"
#include "Exceptions/Generic_Exception.h"
#include "Exceptions/Promotion_Exception.h"
#include "Exceptions/Out_Of_Time_Exception.h"

#include "Utility.h"

Board::Board() :
    board(64, Square()),
    turn_color(WHITE),
    winner(NONE),
    is_original(true)
{
    Color color = WHITE;
    while(true)
    {
        int base_rank = (color == WHITE ? 1 : 8);
        place_piece(std::make_shared<Rook>(color), 'a', base_rank);
        place_piece(std::make_shared<Knight>(color), 'b', base_rank);
        place_piece(std::make_shared<Bishop>(color), 'c', base_rank);
        place_piece(std::make_shared<Queen>(color), 'd', base_rank);
        place_piece(std::make_shared<King>(color), 'e', base_rank);
        place_piece(std::make_shared<Bishop>(color), 'f', base_rank);
        place_piece(std::make_shared<Knight>(color), 'g', base_rank);
        place_piece(std::make_shared<Rook>(color), 'h', base_rank);
        for(char file = 'a'; file <= 'h'; ++file)
        {
            place_piece(std::make_shared<Pawn>(color), file, (base_rank == 1) ? 2 : 7);
        }

        if(color == BLACK)
        {
            break;
        }

        color = BLACK;
    }
}

Board::Board(const std::string& fen) :
    board(64, Square()),
    turn_color(WHITE),
    winner(NONE),
    is_original(true)
{
    auto fen_parse = String::split(fen);
    auto board_parse = String::split(fen_parse.at(0), "/");
    turn_color = (fen_parse[1] == "w" ? WHITE : BLACK);
    auto castling_parse = fen_parse.at(2);
    auto en_passant_parse = fen_parse.at(3);
    if(en_passant_parse != "-")
    {
        get_square(en_passant_parse[0], en_passant_parse[1] - '0').make_en_passant_targetable();
    }

    for(int rank = 8; rank >= 1; --rank)
    {
        char file = 'a';
        for(const auto& symbol : board_parse.at(8-rank))
        {
            if(isdigit(symbol))
            {
                file += symbol - '0';
            }
            else
            {
                Color color = (isupper(symbol) ? WHITE : BLACK);
                switch(toupper(symbol))
                {
                    case 'P':
                        place_piece(std::make_shared<Pawn>(color), file, rank);
                        break;
                    case 'R':
                        place_piece(std::make_shared<Rook>(color), file, rank);
                        break;
                    case 'N':
                        place_piece(std::make_shared<Knight>(color), file, rank);
                        break;
                    case 'B':
                        place_piece(std::make_shared<Bishop>(color), file, rank);
                        break;
                    case 'Q':
                        place_piece(std::make_shared<Queen>(color), file, rank);
                        break;
                    case 'K':
                        place_piece(std::make_shared<King>(color), file, rank);
                        break;
                    default:
                        throw Generic_Exception(std::string("Invalid  symbol in FEN string: ") + symbol);
                }
                ++file;
            }
        }
    }
    for(int rank = 3; rank <= 6; ++rank)
    {
        for(char file = 'a'; file <= 'h'; ++file)
        {
            if( ! view_square(file, rank).empty())
            {
                get_square(file, rank).player_moved_piece();
            }
        }
    }

    if( ! String::contains(castling_parse, 'K'))
    {
        get_square('h', 1).player_moved_piece();
    }
    if( ! String::contains(castling_parse, 'Q'))
    {
        get_square('a', 1).player_moved_piece();
    }
    if( ! String::contains(castling_parse, 'k'))
    {
        get_square('h', 8).player_moved_piece();
    }
    if( ! String::contains(castling_parse, 'q'))
    {
        get_square('a', 8).player_moved_piece();
    }
}

const Square& Board::view_square(char file, int rank) const
{
    // Square A1 = Board::board[0]
    // Square H1 = Board::board[7]
    // Square A8 = Board::board[56]
    // Square H8 = Board::board[63]
    return board.at((file - 'a') + 8*(rank - 1));
}

Square& Board::get_square(char file, int rank)
{
    // see const version
    return board.at((file - 'a') + 8*(rank - 1));
}

bool Board::inside_board(char file)
{
    return file >= 'a' && file <= 'h';
}

bool Board::inside_board(int rank)
{
    return rank >= 1 && rank <= 8;
}

bool Board::is_legal(const Complete_Move& move, bool king_check) const
{
    return is_legal(move.starting_file, move.starting_rank, move.move, king_check);
}

bool Board::is_legal(char file_start, int rank_start, const std::shared_ptr<const Move>& move, bool king_check) const
{
    if( ! move)
    {
        return false;
    }

    char file_end = file_start + move->file_change();
    int rank_end = rank_start + move->rank_change();

    // starting or ending square is outside board
    if( ! (inside_board(file_start) && inside_board(rank_start)
            && inside_board(file_end)   && inside_board(rank_end)))
    {
        return false;
    }
    // Piece-move compatibility
    auto moving_piece = view_square(file_start, rank_start).piece_on_square();
    if( ! moving_piece
            || moving_piece->color() != whose_turn()
            || ! moving_piece->can_move(move))
    {
        return false;
    }

    // Move-specific legality
    if( ! move->is_legal(*this, file_start, rank_start))
    {
        return false;
    }

    // Check that there are no intervening pieces for straight-line moves
    if(move->file_change() == 0
            || move->rank_change() == 0
            || abs(move->file_change()) == abs(move->rank_change()))
    {
        int max_move = std::max(abs(move->file_change()), abs(move->rank_change()));
        int file_step = move->file_change()/max_move;
        int rank_step = move->rank_change()/max_move;

        for(int step = 1; step < max_move; ++step)
        {
            if( ! view_square(file_start + file_step*step,
                              rank_start + rank_step*step).empty())
            {
                return false;
            }
        }
    }

    // Cannot capture piece of same color
    auto attacked_piece = view_square(file_end, rank_end).piece_on_square();
    if(attacked_piece && moving_piece->color() == attacked_piece->color())
    {
        return false;
    }

    // King should not be in check after move
    if(king_check)
    {
        Board trial(*this);
        trial.make_move(file_start,                       rank_start,
                        file_start + move->file_change(), rank_start + move->rank_change());
        move->side_effects(trial, file_start, rank_start);
        return ! trial.king_is_in_check(whose_turn());
    }

    return true;
}

bool Board::is_legal(char file_start, int rank_start,
                     char file_end,   int rank_end, bool king_check) const
{
    // Find move that moves piece from start square to end square
    auto piece = view_square(file_start, rank_start).piece_on_square();
    return piece && piece->get_legal_moves(*this, file_start, rank_start,
                                                  file_end,   rank_end,
                                                  king_check).size() > 0;
}

std::string Board::fen_status() const
{
    std::string s;
    int empty_count = 0;
    for(int rank = 8; rank >= 1; --rank)
    {
        if(rank < 8)
        {
            s.append("/");
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            const Square& square = view_square(file, rank);
            if(square.empty())
            {
                ++empty_count;
            }
            else
            {
                if(empty_count > 0)
                {
                    s.append(String::to_string(empty_count));
                    empty_count = 0;
                }
                s.push_back(square.piece_on_square()->fen_symbol());
            }
        }
        if(empty_count > 0)
        {
            s.append(String::to_string(empty_count));
            empty_count = 0;
        }
    }

    s.push_back(' ');
    s.push_back(tolower(color_text(whose_turn())[0]));
    s.push_back(' ');

    for(int base_rank = 1; base_rank <= 8; base_rank += 7)
    {
        auto king_square = view_square('e', base_rank);
        if( ! king_square.empty() && ! king_square.piece_has_moved())
        {
            for(char castle_file = 'h'; castle_file >= 'a'; castle_file -= 7)
            {
                auto castle_square = view_square(castle_file, base_rank);
                if( ! castle_square.empty() && ! castle_square.piece_has_moved())
                {
                    std::string mark = (castle_file == 'h' ? "K" : "Q");
                    if(base_rank == 8)
                    {
                        mark[0] = tolower(mark[0]);
                    }
                    s.append(mark);
                }
            }
        }
    }
    if(s.back() == ' ')
    {
        s.push_back('-');
    }
    s.push_back(' ');

    for(char file = 'a'; file <= 'h'; file++)
    {
        for(int rank = 3; rank <= 6; rank += 3) // en passant capture only possible on these ranks
        {
            if(view_square(file, rank).is_en_passant_targetable())
            {
                s.push_back(file);
                s.append(String::to_string(rank));
            }
        }
    }
    if(s.back() == ' ')
    {
        s.push_back('-');
    }

    // ignoring move count since this is kept by the board itself

    return s;
}


Complete_Move Board::get_complete_move(char file_start, int rank_start, char file_end, int rank_end, char promote) const
{
    auto piece = view_square(file_start, rank_start).piece_on_square();
    if(piece == nullptr)
    {
        throw Illegal_Move_Exception("No piece on square " +
                                     std::string(1, file_start) +
                                     String::to_string(rank_start));
    }

    auto move_list = piece->get_legal_moves(*this, file_start, rank_start,
                                      file_end,   rank_end, true, promote);

    if(move_list.empty())
    {
        throw Illegal_Move_Exception("No legal move found for " +
                                     std::string(1, file_start) +
                                     String::to_string(rank_start) +
                                     "-" +
                                     std::string(1, file_end) +
                                     String::to_string(rank_end) +
                                     " (" + String::to_string(promote) + ")");
    }

    if(move_list.size() == 1)
    {
        Complete_Move result;
        result.move = move_list.front();
        result.starting_file = file_start;
        result.starting_rank = rank_start;
        return result;
    }
    else if(promote)
    {
        for(const auto& move : move_list)
        {
            if(tolower(move->name().back()) == tolower(promote))
            {
                Complete_Move result;
                result.move = move;
                result.starting_file = file_start;
                result.starting_rank = rank_start;
                return result;
            }
        }

        throw Illegal_Move_Exception("No suitable promotion to " + std::string(1, promote));
    }
    else
    {
        throw Promotion_Exception();
    }
}

void Board::submit_move(const Complete_Move& cm)
{
    submit_move(cm.starting_file, cm.starting_rank, cm.move);
}

void Board::submit_move(char file_start, int rank_start, const std::shared_ptr<const Move>& move)
{
    if( ! is_legal(file_start, rank_start, move, true))
    {
        throw Illegal_Move_Exception("Illegal move: ." +
                                     std::string(1, file_start) + "." +
                                     String::to_string(rank_start) +
                                     " " +
                                     move->name());
    }

    if(game_record.empty() && whose_turn() == BLACK)
    {
        game_record.push_back("...");
    }
    game_record.push_back(move->game_record_item(*this, file_start, rank_start));

    make_move(file_start,                       rank_start,
              file_start + move->file_change(), rank_start + move->rank_change());
    move->side_effects(*this, file_start, rank_start);

    all_moves_cache.clear();
    all_legal_moves_cache.clear();

    if(king_is_in_check(opposite(turn_color)))
    {
        game_record.back().append("+");
    }

    turn_color = opposite(turn_color);

    if(no_legal_moves())
    {
        if(king_is_in_check(whose_turn()))
        {
            game_record.back().append("+");
            set_winner(opposite(whose_turn()));
            if(get_winner() == WHITE)
            {
                game_record.back().append("\t1-0");
            }
            else
            {
                game_record.back().append("\t0-1");
            }
            throw Checkmate_Exception(opposite(whose_turn()));
        }
        else
        {
            game_record.back().append("\t1/2-1/2");
            throw Stalemate_Exception(color_text(whose_turn()) +
                                      " has no legal moves but is not in check. Game is stalemated.");
        }
    }

    if(++repeat_count[board_status()] >= 3)
    {
        game_record.back().append("\t1/2-1/2");
        throw Stalemate_Exception("Threefold repitition");
    }

    int fifty_move_count = 0;
    for(const auto& board_count : repeat_count)
    {
        fifty_move_count += board_count.second;
    }
    if(fifty_move_count >= 100) // "Move" means both players move.
    {
        game_record.back().append("\t1/2-1/2");
        throw Stalemate_Exception("50-move limit");
    }
}

Complete_Move Board::get_complete_move(const std::string& move, char promote) const
{
    const std::string valid_files = "abcdefgh";
    const std::string valid_rows = "12345678";
    const std::string promotions = "qbnr";
    const std::string valid_characters = valid_files + valid_rows + promotions;

    std::string validated;
    for(char c : move)
    {
        c = tolower(c);
        if(String::contains(valid_characters, c))
        {
            validated.push_back(c);
        }
    }

    if(validated.size() < 4 || validated.size() > 5)
    {
        throw Illegal_Move_Exception("Illegal text move: " + move);
    }


    char start_file = validated[0];
    int  start_rank = validated[1] - '0';
    char end_file   = validated[2];
    int  end_rank   = validated[3] - '0';
    if(validated.size() == 5)
    {
        promote = validated[4];
    }

    return get_complete_move(start_file, start_rank, end_file, end_rank, promote);
}

void Board::make_move(char file_start, int rank_start, char file_end, int rank_end)
{
    if(get_square(file_end, rank_end).piece_on_square()) // capture
    {
        repeat_count.clear();
    }
    place_piece(get_square(file_start, rank_start).piece_on_square(), file_end, rank_end);
    place_piece(nullptr, file_start, rank_start);
    get_square(file_start, rank_start).player_moved_piece();
    get_square(file_end,   rank_end  ).player_moved_piece();
    for(int rank = 3; rank <= 6; rank += 3)
    {
        for(char file = 'a'; file <= 'h'; ++file)
        {
            get_square(file, rank).remove_en_passant_targetable();
        }
    }
}

Color Board::whose_turn() const
{
    return turn_color;
}

std::vector<Complete_Move> Board::all_legal_moves() const
{
    if( ! all_legal_moves_cache.empty())
    {
        return all_legal_moves_cache;
    }

    for(const auto& complete_move : all_moves())
    {
        if(is_legal(complete_move))
        {
            all_legal_moves_cache.push_back(complete_move);
        }
    }

    return all_legal_moves_cache;
}

void Board::ascii_draw(Color color) const
{
    const int square_width = 7;
    const int square_height = 3;

    const std::string square_corner = "+";
    const std::string square_horizontal_border = "-";
    const std::string square_vertical_border = "|";

    std::string horizontal_line;
    for(int i = 0; i < 8; ++i)
    {
        horizontal_line.append(square_corner);

        for(int j = 0; j < square_width; ++j)
        {
            horizontal_line.append(square_horizontal_border);
        }
    }
    horizontal_line.append(square_corner + "\n");
    const std::string left_spacer = "   ";

    int rank_start = (color == WHITE ? 8 : 1);
    int rank_end = (color == WHITE ? 1 : 8);
    int d_rank = (color == WHITE ? -1 : 1);
    char file_start = (color == WHITE ? 'a' : 'h');
    char file_end = (color == WHITE ? 'h' : 'a');
    int d_file = (color == WHITE ? 1 : -1);

    for(int rank = rank_start; d_rank*(rank_end - rank) >= 0; rank += d_rank)
    {
        std::cout << left_spacer << horizontal_line;

        for(int square_row = 0; square_row < square_height; ++square_row)
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
                auto piece = view_square(file, rank).piece_on_square();
                char dark_square_fill = ':';
                char filler = (((file + rank)%2 == (file_start + rank_start)%2) ? ' ' : dark_square_fill);
                if(piece)
                {
                    auto piece_row = piece->ascii_art(square_row);
                    std::string padding((square_width - piece_row.size())/2, filler);
                    piece_symbol = padding + piece_row + padding;
                }
                else
                {
                    piece_symbol.append(square_width, filler);
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

void Board::place_piece(const std::shared_ptr<const Piece>& p, char file, int rank)
{
    get_square(file, rank).place_piece(p);
}

bool Board::king_is_in_check(Color king_color) const
{
    // find king of input color
    for(int rank = 1; rank <= 8; ++rank)
    {
        for(char file = 'a'; file <= 'h'; ++file)
        {
            auto piece = view_square(file, rank).piece_on_square();
            if(piece && piece->color() == king_color &&  piece->pgn_symbol() == "K")
            {
                return square_attacked_by(file, rank, opposite(king_color));
            }
        }
    }
    ascii_draw(WHITE);
    print_game_record("", "");
    throw Generic_Exception(std::string("Error in Board::king_is_in_check(): missing ")
                            + color_text(king_color) + " King.");
}

bool Board::square_attacked_by(char file, int rank, Color color) const
{
    auto temp = make_hypothetical();
    temp.set_turn(color);

    for(char file_start = 'a'; file_start <= 'h'; ++file_start)
    {
        for(int rank_start = 1; rank_start <= 8; ++rank_start)
        {
            if(temp.is_legal(file_start, rank_start, file, rank, false))
            {
                return true;
            }
        }
    }

    return false;
}

bool Board::no_legal_moves() const
{
    return all_legal_moves().empty();
}

void Board::print_game_record(const std::string& white_name,
                              const std::string& black_name,
                              const std::string& file_name,
                              const std::string& outside_result) const
{
    std::string result;
    std::string termination;
    if( ! outside_result.empty())
    {
        auto split = String::split(outside_result, " ", 1);
        result = split[0];
        termination = split[1];
    }

    if( ! game_record.empty())
    {
        auto tab_split = String::split(game_record.back(), "\t", 1);
        if(tab_split.size() > 1)
        {
            result = tab_split[1];
        }
    }

    Color c = WHITE;
    int step = 0;
    std::ostringstream oss;
    oss << "[White \"" << white_name << "\"]\n";
    oss << "[Black \"" << black_name << "\"]\n";
    oss << "[Result \"" << result << "\"]\n";
    if( ! termination.empty())
    {
        oss << "[Termination \"" << termination << "\"]\n";
    }
    for(const auto& record : game_record)
    {
        if(c == WHITE)
        {
            oss << "\n" << ++step << ".";
        }
        oss << " " << record;
        c = opposite(c);
    }
    oss << '\n';

    if(file_name.empty())
    {
        std::cout << oss.str();
    }
    else
    {
        std::ofstream(file_name, std::ios::app) << oss.str() << std::endl << std::endl;
    }
}

std::string Board::board_status() const // for 3-fold rep count
{
    std::string s;
    for(int rank = 1; rank <= 8; ++rank)
    {
        for(char file = 'a'; file <= 'h'; ++file)
        {
            // append name of piece on square
            auto piece = view_square(file, rank).piece_on_square();
            if(piece)
            {
                s.append(String::to_string(piece->fen_symbol()));

                // write legal destination squares for this piece
                for(auto move : piece->get_move_list())
                {
                    if(is_legal(file, rank, move))
                    {
                        s.append(String::to_string(move->file_change()));
                        s.append(String::to_string(move->rank_change()));
                    }
                }
            }
            else
            {
                s.append(".");
            }
            s.append(String::to_string(char(color_text(whose_turn())[0])));
        }
    }

    return s;
}

Color Board::get_winner() const
{
    return winner;
}

void Board::set_winner(Color color)
{
    winner = color;
}

unsigned int Board::number_of_moves() const
{
    return game_record.size();
}

std::string Board::last_move() const
{
    if(game_record.size() > 0)
    {
        return game_record.back();
    }
    else
    {
        return "";
    }
}

Board Board::make_hypothetical() const
{
    Board new_board(*this);
    new_board.is_original = false;
    return new_board;
}

void Board::set_turn(Color color)
{
    if(is_original)
    {
        throw Generic_Exception("Cannot call set_turn() on original board.");
    }

    if(turn_color != color)
    {
        all_legal_moves_cache.clear();
        all_moves_cache.clear();
    }
    turn_color = color;
}

int Board::number_all_moves() const
{
    int result = 0;

    for(const auto& square : board)
    {
        auto piece = square.piece_on_square();
        if(piece)
        {
            result += piece->get_move_list().size();
        }
    }

    return result;
}

std::vector<Complete_Move> Board::all_moves() const
{
    if( ! all_moves_cache.empty())
    {
        return all_moves_cache;
    }

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = view_square(file, rank).piece_on_square();

            if(piece)
            {
                for(const auto& move : piece->get_move_list())
                {
                    Complete_Move cm{move, file, rank};
                    all_moves_cache.emplace_back(cm);
                }
            }
        }
    }

    return all_moves_cache;
}
