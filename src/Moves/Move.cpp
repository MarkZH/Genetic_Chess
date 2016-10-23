#include "Moves/Move.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"
#include "Exceptions/Illegal_Move_Exception.h"


Move::Move(int d_file_in, int d_rank_in) : d_file(d_file_in), d_rank(d_rank_in)
{
    if(std::abs(d_file) >= 8 ||
       std::abs(d_rank) >= 8 ||
       (d_file == 0 && d_rank == 0))
    {
        throw Illegal_Move_Exception("Badly constructed move: " +
                                     std::to_string(d_file) +
                                     " - "
                                     + std::to_string(d_rank));
    }
}

Move::~Move()
{
}

void Move::side_effects(Board&, char /* file_start */, int /* rank_start */) const
{
}

bool Move::is_legal(const Board& /* board */, char /* file_start */, int /* rank_start*/) const
{
    return true;
}

int Move::file_change() const
{
    return d_file;
}

int Move::rank_change() const
{
    return d_rank;
}

std::string Move::name() const
{
    return "Move";
}

std::string Move::game_record_item(const Board& board, char file_start, int rank_start) const
{
    char file_end = file_start + file_change();
    int  rank_end = rank_start + rank_change();

    auto original_piece = board.piece_on_square(file_start, rank_start);
    std::string move_record = original_piece->pgn_symbol();

    bool record_file = false;
    bool record_rank = false;
    for(char file_other = 'a'; file_other <= 'h'; ++file_other)
    {
        for(int rank_other = 1; rank_other <= 8; ++rank_other)
        {
            if( ! board.piece_on_square(file_other, rank_other))
            {
                continue;
            }
            if(file_other == file_start && rank_other == rank_start)
            {
                continue;
            }
            if(file_other == file_end && rank_other == rank_end)
            {
                continue;
            }
            auto new_piece = board.piece_on_square(file_other, rank_other);
            if((*original_piece) != (*new_piece))
            {
                continue;
            }

            if(board.is_legal(file_other, rank_other, file_end, rank_end))
            {
                if(file_other != file_start && ! record_file)
                {
                    record_file = true;
                    continue;
                }
                if(rank_other != rank_start)
                {
                    record_rank = true;
                }
            }
        }
    }

    if(record_file)
    {
        move_record.append(std::string(1, file_start));
    }
    if(record_rank)
    {
        move_record.append(std::to_string(rank_start));
    }

    if(board.piece_on_square(file_end, rank_end))
    {
        move_record.append("x");
    }

    move_record.append(std::string(1, file_end));
    move_record.append(std::to_string(rank_end));

    return move_record;
}
