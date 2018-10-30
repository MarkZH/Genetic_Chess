#include "Pieces/Piece.h"

#include <cctype>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>

#include "Game/Board.h"
#include "Pieces/Piece_Types.h"

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Moves/Pawn_Move.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Pawn_Double_Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Pawn_Promotion_by_Capture.h"
#include "Moves/En_Passant.h"
#include "Moves/Castle.h"

Piece::Piece(Color color_in, Piece_Type type_in) :
    my_color(color_in),
    my_type(type_in)
{
    switch(type())
    {
        case PAWN:
            add_pawn_moves();
            break;
        case ROOK:
            add_rook_moves();
            break;
        case KNIGHT:
            add_knight_moves();
            break;
        case BISHOP:
            add_bishop_moves();
            break;
        case QUEEN:
            add_bishop_moves();
            add_rook_moves();
            break;
        case KING:
            add_king_moves();
            break;
        default:
            throw std::runtime_error("Program bug: Invalid piece type in Piece(): " + std::to_string(type()));
    }
 
}

Color Piece::color() const
{
    return my_color;
}

std::string Piece::pgn_symbol() const
{
    return type() == PAWN ? std::string{} : std::string(1, std::toupper(fen_symbol()));
}

char Piece::fen_symbol() const
{
    char symbol;
    switch(type())
    {
        case PAWN:
            symbol = 'P';
            break;
        case ROOK:
            symbol = 'R';
            break;
        case KNIGHT:
            symbol = 'N';
            break;
        case BISHOP:
            symbol = 'B';
            break;
        case QUEEN:
            symbol = 'Q';
            break;
        case KING:
            symbol = 'K';
            break;
        default:
            throw std::runtime_error("Program bug: Invalid piece type in Piece::fen_symbol(): " + std::to_string(type()));
    }
    return (my_color == WHITE ? symbol : std::tolower(symbol));
}

bool Piece::can_move(const Move* move) const
{
    return std::find_if(possible_moves.begin(),
                        possible_moves.end(),
                        [move](const auto& x){ return x.get() == move; }) != possible_moves.end();
}

const std::vector<const Move*>& Piece::move_list(char file, int rank) const
{
    return legal_moves[Board::square_index(file, rank)];
}

Piece_Type Piece::type() const
{
    return my_type;
}

void Piece::add_standard_legal_move(int file_step, int rank_step)
{
    for(char start_file = 'a'; start_file <= 'h'; ++start_file)
    {
        for(int start_rank = 1; start_rank <= 8; ++start_rank)
        {
            char end_file = start_file + file_step;
            int  end_rank = start_rank + rank_step;

            add_legal_move(std::make_unique<Move>(start_file, start_rank,
                                                  end_file, end_rank));
        }
    }
}

void Piece::add_legal_move(std::unique_ptr<Move> move)
{
    assert(Board::inside_board(move->start_file(), move->start_rank()));
    if(Board::inside_board(move->end_file(), move->end_rank()))
    {
        legal_moves[Board::square_index(move->start_file(), move->start_rank())].push_back(move.get());
        possible_moves.push_back(std::move(move));
    }
}

void Piece::add_pawn_moves()
{
    // Ranks where pawns can exist
    auto base_rank = (color() == WHITE ? 2 : 7);

    // Pawns cannot make a normal move from this rank without promoting
    auto no_normal_move_rank = (color() == WHITE ? 7 : 2);
    auto direction = (color() == WHITE ? 1 : -1);
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
        {
            add_legal_move(std::make_unique<Pawn_Move>(color(), file, rank));
        }
    }

    for(char file = 'a'; file <= 'h'; ++file)
    {
        add_legal_move(std::make_unique<Pawn_Double_Move>(color(), file));
    }

    std::vector<const Piece*> possible_promotions;
    possible_promotions.emplace_back(Board::piece_instance(QUEEN, color()));
    possible_promotions.emplace_back(Board::piece_instance(KNIGHT, color()));
    possible_promotions.emplace_back(Board::piece_instance(ROOK, color()));
    possible_promotions.emplace_back(Board::piece_instance(BISHOP, color()));

    for(auto dir : {RIGHT, LEFT})
    {
        auto first_file = (dir == RIGHT ? 'a' : 'b');
        auto last_file = (dir == RIGHT ? 'g' : 'h');
        for(char file = first_file; file <= last_file; ++file)
        {
            for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
            {
                add_legal_move(std::make_unique<Pawn_Capture>(color(), dir, file, rank));
            }
        }

        auto en_passant_rank = (color() == WHITE ? 5 : 4);
        for(char file = first_file; file <= last_file; ++file)
        {
            add_legal_move(std::make_unique<En_Passant>(color(), dir, file, en_passant_rank));
        }

        for(auto promote : possible_promotions)
        {
            for(auto file = first_file; file <= last_file; ++file)
            {
                add_legal_move(std::make_unique<Pawn_Promotion_by_Capture>(promote, dir, file));
            }
        }
    }

    for(auto promote : possible_promotions)
    {
        for(auto file = 'a'; file <= 'h'; ++file)
        {
            add_legal_move(std::make_unique<Pawn_Promotion>(promote, file));
        }
    }
}

void Piece::add_rook_moves()
{
    for(int d_file = -1; d_file <= 1; ++d_file)
    {
        for(int d_rank = -1; d_rank <= 1; ++d_rank)
        {
            if(d_file == 0 && d_rank == 0) { continue; }
            if(d_file != 0 && d_rank != 0) { continue; }

            for(int move_size = 1; move_size <= 7; ++move_size)
            {
                add_standard_legal_move(move_size*d_file, move_size*d_rank);
            }
        }
    }
}

void Piece::add_knight_moves()
{
    for(auto d_file : {1, 2})
    {
        auto d_rank = 3 - d_file;
        for(auto file_direction : {-1, 1})
        {
            for(auto rank_direction : {-1, 1})
            {
                add_standard_legal_move(d_file*file_direction, d_rank*rank_direction);
            }
        }
    }
}

void Piece::add_bishop_moves()
{
    for(int d_rank : {-1, 1})
    {
        for(int d_file : {-1, 1})
        {
            for(int move_size = 1; move_size <= 7; ++move_size)
            {
                add_standard_legal_move(move_size*d_file, move_size*d_rank);
            }
        }
    }
}

void Piece::add_king_moves()
{
    for(int d_rank = -1; d_rank <= 1; ++d_rank)
    {
        for(int d_file = -1; d_file <= 1; ++d_file)
        {
            if(d_rank == 0 && d_file == 0) { continue; }

            add_standard_legal_move(d_file, d_rank);
        }
    }

    int base_rank = (color() == WHITE ? 1 : 8);
    add_legal_move(std::make_unique<Castle>(base_rank, LEFT));
    add_legal_move(std::make_unique<Castle>(base_rank, RIGHT));
}
