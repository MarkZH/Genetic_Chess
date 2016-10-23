#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Game/Board.h"

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
#include "Exceptions/Promotion_Exception.h"

#include "Utility.h"

Board::Board() :
    board(64, nullptr),
    turn_color(WHITE),
    winner(NONE),
    is_original(true),
    en_passant_target_file('\0'),
    en_passant_target_rank(0)
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

    all_pieces_unmoved();
}

Board::Board(const std::string& fen) :
    board(64, nullptr),
    turn_color(WHITE),
    winner(NONE),
    is_original(true),
    en_passant_target_file('\0'),
    en_passant_target_rank(0)
{
    auto fen_parse = String::split(fen);
    auto board_parse = String::split(fen_parse.at(0), "/");
    turn_color = (fen_parse[1] == "w" ? WHITE : BLACK);
    auto castling_parse = fen_parse.at(2);
    auto en_passant_parse = fen_parse.at(3);
    if(en_passant_parse != "-")
    {
        make_en_passant_targetable(en_passant_parse[0], en_passant_parse[1] - '0');
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
                        throw std::runtime_error(std::string("Invalid  symbol in FEN string: ") + symbol);
                }
                ++file;
            }
        }
    }
    all_pieces_unmoved();

    // mark pawns not on starting squares as moved
    for(int rank = 1; rank <= 8; ++rank)
    {
        for(char file = 'a'; file <= 'h'; ++file)
        {
            auto piece = piece_on_square(file, rank);
            if(piece)
            {
                if((piece->fen_symbol() == 'P' && rank != 2) ||
                   (piece->fen_symbol() == 'p' && rank != 7))
                {
                    piece_moved[piece] = true;
                }
            }
        }
    }

    if( ! String::contains(castling_parse, 'K'))
    {
        piece_moved[piece_on_square('h', 1)] = true;
    }
    if( ! String::contains(castling_parse, 'Q'))
    {
        piece_moved[piece_on_square('a', 1)] = true;
    }
    if( ! String::contains(castling_parse, 'k'))
    {
        piece_moved[piece_on_square('h', 8)] = true;
    }
    if( ! String::contains(castling_parse, 'q'))
    {
        piece_moved[piece_on_square('a', 8)] = true;
    }
}

std::shared_ptr<const Piece>& Board::piece_on_square(char file, int rank)
{
    // Square A1 = Board::board[0]
    // Square H1 = Board::board[7]
    // Square A8 = Board::board[56]
    // Square H8 = Board::board[63]
    return board[(file - 'a') + 8*(rank - 1)];
}

const std::shared_ptr<const Piece>& Board::piece_on_square(char file, int rank) const
{
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
    auto moving_piece = piece_on_square(file_start, rank_start);
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
    // if(...) conditional excludes checking knight moves
    if(move->file_change() == 0
            || move->rank_change() == 0
            || abs(move->file_change()) == abs(move->rank_change()))
    {
        int max_move = std::max(abs(move->file_change()), abs(move->rank_change()));
        int file_step = move->file_change()/max_move;
        int rank_step = move->rank_change()/max_move;

        for(int step = 1; step < max_move; ++step)
        {
            if(piece_on_square(file_start + file_step*step,
                               rank_start + rank_step*step))
            {
                return false;
            }
        }
    }

    // Cannot capture piece of same color
    auto attacked_piece = piece_on_square(file_end, rank_end);
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
    auto piece = piece_on_square(file_start, rank_start);
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
            auto piece = piece_on_square(file, rank);
            if( ! piece)
            {
                ++empty_count;
            }
            else
            {
                if(empty_count > 0)
                {
                    s.append(std::to_string(empty_count));
                    empty_count = 0;
                }
                s.push_back(piece->fen_symbol());
            }
        }
        if(empty_count > 0)
        {
            s.append(std::to_string(empty_count));
            empty_count = 0;
        }
    }

    s.push_back(' ');
    s.push_back(tolower(color_text(whose_turn())[0]));
    s.push_back(' ');

    for(int base_rank : {1, 8})
    {
        if( ! piece_has_moved('e', base_rank)) // has king moved?
        {
            for(char rook_file : {'h', 'a'})
            {
                if( ! piece_has_moved(rook_file, base_rank)) // have rooks moved
                {
                    std::string mark = (rook_file == 'h' ? "K" : "Q");
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
        for(int rank : {3, 6}) // en passant capture only possible on these ranks
        {
            if(is_en_passant_targetable(file, rank))
            {
                s.push_back(file);
                s.append(std::to_string(rank));
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
    auto piece = piece_on_square(file_start, rank_start);
    if(piece == nullptr)
    {
        throw Illegal_Move_Exception("No piece on square " +
                                     std::string(1, file_start) +
                                     std::to_string(rank_start));
    }

    auto move_list = piece->get_legal_moves(*this, file_start, rank_start,
                                      file_end,   rank_end, true, promote);

    if(move_list.empty())
    {
        throw Illegal_Move_Exception("No legal move found for " +
                                     std::string(1, file_start) +
                                     std::to_string(rank_start) +
                                     "-" +
                                     std::string(1, file_end) +
                                     std::to_string(rank_end) +
                                     " (" + std::string(1, promote) + ")");
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
                                     std::to_string(rank_start) +
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
    if(piece_on_square(file_end, rank_end)) // capture
    {
        repeat_count.clear();
    }
    auto piece = piece_on_square(file_start, rank_start);
    place_piece(piece, file_end, rank_end);
    place_piece(nullptr, file_start, rank_start);
    clear_en_passant_target();
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

void Board::ascii_draw(Color perspective) const
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

    int rank_start = (perspective == WHITE ? 8 : 1);
    int rank_end = (perspective == WHITE ? 1 : 8);
    int d_rank = (perspective == WHITE ? -1 : 1);
    char file_start = (perspective == WHITE ? 'a' : 'h');
    char file_end = (perspective == WHITE ? 'h' : 'a');
    int d_file = (perspective == WHITE ? 1 : -1);

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
                auto piece = piece_on_square(file, rank);
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
    piece_on_square(file, rank) = p;
    piece_moved[p] = true;
}

bool Board::king_is_in_check(Color king_color) const
{
    // find king of input color
    auto king_location = find_king(king_color);
    char file = king_location.first;
    int  rank = king_location.second;
    return square_attacked_by(file, rank, opposite(king_color));
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

const std::vector<std::string>& Board::get_game_record() const
{
    return game_record;
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
        std::cout << oss.str() << std::endl;
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
            auto piece = piece_on_square(file, rank);
            if(piece)
            {
                s.append(std::string(1, piece->fen_symbol()));

                // write legal destination squares for this piece
                for(auto move : piece->get_move_list())
                {
                    if(is_legal(file, rank, move))
                    {
                        s.append(std::to_string(move->file_change()));
                        s.append(std::to_string(move->rank_change()));
                    }
                }
            }
            else
            {
                s.append(".");
            }
            s.append(color_text(whose_turn()));
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
        throw std::runtime_error("Cannot call set_turn() on original board.");
    }

    if(turn_color != color)
    {
        all_legal_moves_cache.clear();
        all_moves_cache.clear();
    }
    turn_color = color;
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
            auto piece = piece_on_square(file, rank);
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

void Board::make_en_passant_targetable(char file, int rank)
{
    en_passant_target_file = file;
    en_passant_target_rank = rank;
}

bool Board::is_en_passant_targetable(char file, int rank) const
{
    return en_passant_target_file == file &&
           en_passant_target_rank == rank;
}

void Board::clear_en_passant_target()
{
    en_passant_target_file = '\0';
    en_passant_target_rank = 0;
}

bool Board::piece_has_moved(char file, int rank) const
{
    return piece_has_moved(piece_on_square(file, rank));
}

bool Board::piece_has_moved(const std::shared_ptr<const Piece>& piece) const
{
    return piece_moved.at(piece);
}

void Board::all_pieces_unmoved()
{
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            piece_moved[piece_on_square(file, rank)] = false;
        }
    }

    // If a square is empty, whatever piece you were looking for
    // must have moved away.
    piece_moved[nullptr] = true;
}

std::pair<char, int> Board::find_king(Color color) const
{
    char king_file;
    int  king_rank;
    bool king_found = false;
    for(king_file = 'a'; king_file <= 'h'; ++king_file)
    {
        for(king_rank = 1; king_rank <= 8; ++king_rank)
        {
            auto piece = piece_on_square(king_file, king_rank);
            if(piece && piece->color() == color && piece->pgn_symbol() == "K")
            {
                king_found = true;
                break;
            }
        }
        if(king_found) { break; }
    }

    if( ! king_found)
    {
        ascii_draw(WHITE);
        throw std::runtime_error(color_text(color) + " king not found on board.");
    }

    return std::make_pair(king_file, king_rank);
}
