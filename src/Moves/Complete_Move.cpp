#include "Moves/Complete_Move.h"

#include <string>

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Exceptions/Game_Ending_Exception.h"

Complete_Move::Complete_Move(const Move* move_in, char file, int rank) :
    move(move_in),
    starting_file(file),
    starting_rank(rank)
{
}

Complete_Move::Complete_Move() :
    move(),
    starting_file('\0'),
    starting_rank(0)
{
}

std::string Complete_Move::game_record_item(const Board& board) const
{
    if(move)
    {
        auto item = move->game_record_item(board, starting_file, starting_rank);
        auto temp = board;
        try
        {
            temp.submit_move(*this);
            if(temp.king_is_in_check(temp.whose_turn()))
            {
                item += '+';
            }
        }
        catch(const Game_Ending_Exception& gee)
        {
            if(gee.winner() == WHITE)
            {
                item += "#\t1-0";
            }
            else if(gee.winner() == BLACK)
            {
                item += "#\t0-1";
            }
            else
            {
                item += "\t1/2-1/2";
            }
        }

        return item;
    }
    else
    {
        return "...";
    }
}

bool Complete_Move::is_legal(const Board& board) const
{
    return move->is_legal(board, starting_file, starting_rank);
}

std::string Complete_Move::coordinate_move() const
{
    return move->coordinate_move(starting_file, starting_rank);
}

std::string Complete_Move::name() const
{
    return std::string(1, start_file()) +
           std::to_string(start_rank()) + " " +
           move->name();
}

char Complete_Move::start_file() const
{
    return starting_file;
}

char Complete_Move::end_file() const
{
    return start_file() + move->file_change();
}

int Complete_Move::start_rank() const
{
    return starting_rank;
}

int Complete_Move::end_rank() const
{
    return starting_rank + move->rank_change();
}

void Complete_Move::side_effects(Board& board) const
{
    move->side_effects(board, start_file(), start_rank());
}

bool Complete_Move::can_capture() const
{
    return move->can_capture();
}

bool Complete_Move::operator==(const Complete_Move& other) const
{
    return start_file() == other.start_file() &&
           start_rank() == other.start_rank() &&
           *move == *other.move;
}
