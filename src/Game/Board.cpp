#include <iostream>
#include <fstream>

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
    en_passant_target_file('\0'),
    en_passant_target_rank(0),
    game_ended(false)
{
    for(auto color : {WHITE, BLACK})
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
    }

    all_pieces_unmoved();
}

Board::Board(const std::string& fen) :
    board(64, nullptr),
    turn_color(WHITE),
    winner(NONE),
    en_passant_target_file('\0'),
    en_passant_target_rank(0),
    game_ended(false)
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
                if(piece->is_pawn() && ((piece->color() == WHITE && rank != 2) ||
                                        (piece->color() == BLACK && rank != 7)))
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

bool Board::inside_board(char file, int rank)
{
    return inside_board(file) && inside_board(rank);
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
    return move && move->is_legal(*this, file_start, rank_start, king_check);
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

    if(en_passant_target_file != '\0' && en_passant_target_rank != 0)
    {
        s.push_back(en_passant_target_file);
        s.push_back(en_passant_target_rank + '0');
    }
    else
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
        return {move_list.front(), file_start, rank_start};
    }
    else if(promote)
    {
        for(const auto& move : move_list)
        {
            if(tolower(move->name().back()) == tolower(promote))
            {
                return {move, file_start, rank_start};
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

    clear_caches();

    if(king_is_in_check(opposite(turn_color)))
    {
        game_record.back().append("+");
    }

    turn_color = opposite(turn_color);

    if(no_legal_moves())
    {
        if(game_record.back().back() == '+') // king in check
        {
            game_record.back().back() = '#';
            set_winner(opposite(whose_turn()));
            if(get_winner() == WHITE)
            {
                game_record.back().append("\t1-0");
            }
            else
            {
                game_record.back().append("\t0-1");
            }
            game_ended = true;
            throw Checkmate_Exception(opposite(whose_turn()));
        }
        else
        {
            game_record.back().append("\t1/2-1/2");
            game_ended = true;
            throw Stalemate_Exception("Stalemate");
        }
    }

    if(++repeat_count[board_status()] >= 3)
    {
        game_record.back().append("\t1/2-1/2");
        game_ended = true;
        throw Stalemate_Exception("Threefold repetition");
    }

    int fifty_move_count = 0;
    for(const auto& board_count : repeat_count)
    {
        fifty_move_count += board_count.second;
    }
    if(fifty_move_count >= 100) // "Move" means both players move.
    {
        game_record.back().append("\t1/2-1/2");
        game_ended = true;
        throw Stalemate_Exception("50-move limit");
    }
}

Complete_Move Board::get_complete_move(const std::string& move, char promote) const
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
    else
    {
        promoted_piece = promote;
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

    if(validated.empty())
    {
        throw Illegal_Move_Exception(move + " does not specify a valid move.");
    }

    // Castling
    if(validated == "OO")
    {
        return get_complete_move('e', whose_turn() == WHITE ? 1 : 8,
                                 'g', whose_turn() == WHITE ? 1 : 8);
    }
    if(validated == "OOO")
    {
        return get_complete_move('e', whose_turn() == WHITE ? 1 : 8,
                                 'c', whose_turn() == WHITE ? 1 : 8);
    }

    // Normal PGN move
    std::string piece_symbol = (String::contains(pieces, validated[0]) ? validated.substr(0, 1) : "");

    char starting_file = 0;
    int  starting_rank = 0;

    char ending_file = validated[validated.size() - 2];
    int  ending_rank = validated[validated.size() - 1] - '0';
    if( ! inside_board(ending_file, ending_rank))
    {
        throw Illegal_Move_Exception("Illegal text move: " + move);
    }

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
    else if((validated.size() == 3 || validated.size() == 2) && piece_symbol.empty()) // Pawn move/capture de5 (dxe5)
    {
        starting_file = validated.front();
    }
    else if(piece_symbol.empty())
    {
        // No PGN-style move works, try coordinate move (e.g., e7e8q)
        if(move.size() < 4 || move.size() > 5)
        {
            throw Illegal_Move_Exception("Illegal text move: " + move);
        }

        char start_file = move[0];
        int  start_rank = move[1] - '0';
        char end_file   = move[2];
        int  end_rank   = move[3] - '0';
        if(move.size() == 5)
        {
            promoted_piece = move[4];
        }

        return get_complete_move(start_file, start_rank, end_file, end_rank, promoted_piece);
    }
    // else normal PGN-style piece movement

    char file_search_start = (starting_file == 0 ? 'a' : starting_file);
    char file_search_end   = (starting_file == 0 ? 'h' : starting_file);

    int rank_search_start = (starting_rank == 0 ? 1 : starting_rank);
    int rank_search_end   = (starting_rank == 0 ? 8 : starting_rank);

    for(char file = file_search_start; file <= file_search_end; ++file)
    {
        for(int rank = rank_search_start; rank <= rank_search_end; ++rank)
        {
            auto piece = piece_on_square(file, rank);
            if(piece &&
               piece->pgn_symbol() == piece_symbol &&
               is_legal(file, rank, ending_file, ending_rank, true))
            {
                if(starting_file == 0 || starting_rank == 0)
                {
                    starting_file = file;
                    starting_rank = rank;
                }
                else
                {
                    throw Illegal_Move_Exception("Ambiguous move: " + move);
                }
            }
        }
    }

    if(starting_file != 0 && starting_rank != 0)
    {
        return get_complete_move(starting_file, starting_rank,
                                 ending_file,   ending_rank,
                                 promoted_piece);
    }

    throw Illegal_Move_Exception("Malformed move: " + move);
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

const std::vector<Complete_Move>& Board::all_legal_moves() const
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
    auto king_square = find_king(king_color);
    return square_attacked_by(king_square.file, king_square.rank, opposite(king_color));
}

bool Board::square_attacked_by(char file, int rank, Color color) const
{
    auto temp = *this;
    temp.set_turn(color);

    for(const auto& move : temp.all_moves())
    {
        if( ! move.move->can_capture())
        {
            continue;
        }

        if(file != move.starting_file + move.move->file_change())
        {
            continue;
        }

        if(rank != move.starting_rank + move.move->rank_change())
        {
            if(move.move->name().front() == 'E')
            {
                // En passant
                if(rank != move.starting_rank)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        if(move.move->is_legal(temp, move.starting_file, move.starting_rank, false))
        {
            return true;
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
                              const std::string& outside_result,
                              unsigned int game_number) const
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

    std::ofstream ofs(file_name, std::ios::app);
    std::ostream& out_stream = (ofs ? ofs : std::cout);
    out_stream << "[White \"" << white_name << "\"]\n";
    out_stream << "[Black \"" << black_name << "\"]\n";
    if(game_number > 0)
    {
        out_stream << "[Round \"" << game_number << "\"]\n";
    }
    out_stream << "[Result \"" << result << "\"]\n";
    if( ! termination.empty())
    {
        out_stream << "[Termination \"" << termination << "\"]\n";
    }

    Color c = WHITE;
    for(size_t i = 0; i < std::max(game_record.size(), game_commentary.size()); ++i)
    {
        if(i < game_record.size())
        {
            auto step = (i + 2)/2;
            if(c == WHITE)
            {
                out_stream << '\n' << step << ".";
            }
            out_stream << " " << game_record.at(i);
        }

        if(i < game_commentary.size() && ! game_commentary.at(i).empty())
        {
            out_stream << " { " << String::trim_outer_whitespace(game_commentary.at(i)) << " }";
        }
        c = opposite(c);
    }
    out_stream << '\n';
}

std::string Board::board_status() const // for 3-fold rep count
{
    auto status = fen_status();
    if(en_passant_target_file == '\0' && en_passant_target_rank == 0)
    {
        return status;
    }

    auto capturing_pawn_rank = (whose_turn() == WHITE ? 6 : 4);
    for(char file : {en_passant_target_file - 1, en_passant_target_file + 1})
    {
        auto possible_pawn = piece_on_square(file, capturing_pawn_rank);
        if(possible_pawn
           && possible_pawn->is_pawn()
           && possible_pawn->color() == whose_turn())
        {
            return status;
        }
    }

    auto status_split = String::split(status);
    return status_split[0] + " " + status_split[1] + " " + status_split[2] + " -";
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
    return game_record.empty() ? std::string() : game_record.back();
}

void Board::set_turn(Color color)
{
    if(turn_color != color)
    {
        clear_caches();
    }
    turn_color = color;
}

const std::vector<Complete_Move>& Board::all_moves() const
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
            if(piece && piece->color() == whose_turn())
            {
                for(const auto& move : piece->get_move_list())
                {
                    all_moves_cache.emplace_back(move, file, rank);
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
    for(const auto& piece : board)
    {
        piece_moved[piece] = false;
    }

    // If a square is empty, whatever piece you were looking for
    // must have moved away.
    piece_moved[nullptr] = true;
}

Square Board::find_king(Color color) const
{
    for(char king_file = 'a'; king_file <= 'h'; ++king_file)
    {
        for(int king_rank = 1; king_rank <= 8; ++king_rank)
        {
            auto piece = piece_on_square(king_file, king_rank);
            if(piece && piece->color() == color && piece->is_king())
            {
                return {king_file, king_rank};
            }
        }
    }

    ascii_draw(WHITE);
    throw std::runtime_error(color_text(color) + " king not found on board.");
}

bool Board::game_has_ended() const
{
    return game_ended;
}

void Board::add_commentary_to_next_move(const std::string& comment) const
{
    while(game_commentary.size() < game_record.size())
    {
        game_commentary.push_back("");
    }
    game_commentary.push_back(comment);
}

void Board::clear_caches()
{
    all_moves_cache.clear();
    all_legal_moves_cache.clear();
}
