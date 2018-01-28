#include "Moves/Threat_Iterator.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"

Threat_Iterator::Threat_Iterator(char file_start,
                                 int  rank_start,
                                 Color attack_color,
                                 const Board& reference_board) :
    starting_file(file_start),
    starting_rank(rank_start),
    file_step(-2),
    rank_step(-2),
    step_size(0),
    attacking_color(attack_color),
    board(reference_board)
{
    next_threat();
}

Threat_Iterator& Threat_Iterator::operator++()
{
    next_threat();
    return *this;
}

Move Threat_Iterator::operator*() const
{
    return Move(ending_file(),
                ending_rank(),
                starting_file,
                starting_rank);
}

bool Threat_Iterator::operator!=(const Threat_Iterator& other) const
{
    return starting_file != other.starting_file ||
           starting_rank != other.starting_rank ||
           file_step != other.file_step ||
           rank_step != other.rank_step ||
           step_size != other.step_size ||
           &board != &other.board;
}

char Threat_Iterator::ending_file() const
{
    return starting_file + file_step*step_size;
}

int Threat_Iterator::ending_rank() const
{
    return starting_rank + rank_step*step_size;
}

bool Threat_Iterator::is_a_threat() const
{
    auto piece = board.piece_on_square(ending_file(), ending_rank());
    if(( ! piece) || piece->color() != attacking_color)
    {
        return false;
    }

    if(piece->is_king())
    {
        return step_size == 1 &&
               std::abs(file_step) <= 1 &&
               std::abs(rank_step) <= 1;
    }

    if(piece->is_pawn())
    {
        return step_size == 1 &&
               std::abs(file_step) == 1 &&
               rank_step == (attacking_color == WHITE ? -1 : 1);
    }

    if(file_step == 0 || rank_step == 0)
    {
        return piece->is_rook() || piece->is_queen();
    }

    if(std::abs(file_step) == std::abs(rank_step))
    {
        return piece->is_bishop() || piece->is_queen();
    }

    return piece->is_knight();
}

void Threat_Iterator::next_threat()
{
    if( ! board.inside_board(ending_file(), ending_rank()) ||
       board.piece_on_square(ending_file(), ending_rank()))
    {
        step_size = 8;
    }

    ++step_size;

    for( ; file_step <= 2; ++file_step)
    {
        for( ; rank_step <= 2; ++rank_step)
        {
            if(file_step == 0 && rank_step == 0)
            {
                continue;
            }

            if(std::abs(file_step) == 2 && std::abs(rank_step) != 1)
            {
                continue;
            }

            if(std::abs(rank_step) == 2 && std::abs(file_step) != 1)
            {
                continue;
            }

            for( ; step_size <= 7; ++step_size)
            {
                if((std::abs(file_step) > 1 || std::abs(rank_step) > 1) && step_size > 1)
                {
                    step_size = 1;
                    break;
                }

                if( ! board.inside_board(ending_file(), ending_rank()))
                {
                    break;
                }

                if(is_a_threat())
                {
                    return;
                }

                auto blocking_piece = board.piece_on_square(ending_file(), ending_rank());
                auto king = (const Piece*)(board.get_king(opposite(attacking_color)));
                if(blocking_piece && blocking_piece != king)
                {
                    break;
                }
            }

            step_size = 1;
        }

        rank_step = -2;
    }

    make_end_iterator();
}

void Threat_Iterator::make_end_iterator()
{
    starting_file = '\0';
    starting_rank = 0;
    file_step = 0;
    rank_step = 0;
    step_size = 0;
}
