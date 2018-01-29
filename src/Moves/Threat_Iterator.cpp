#include "Moves/Threat_Iterator.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"

Threat_Iterator::Threat_Iterator(char target_file_in,
                                 int  target_rank_in,
                                 Color attack_color,
                                 const Board& reference_board) :
    target_file(target_file_in),
    target_rank(target_rank_in),
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
    return Move(attacking_file(),
                attacking_rank(),
                target_file,
                target_rank);
}

bool Threat_Iterator::operator!=(const Threat_Iterator& other) const
{
    return target_file != other.target_file||
           target_rank != other.target_rank ||
           file_step != other.file_step ||
           rank_step != other.rank_step ||
           step_size != other.step_size ||
           &board != &other.board;
}

char Threat_Iterator::attacking_file() const
{
    return target_file + file_step*step_size;
}

int Threat_Iterator::attacking_rank() const
{
    return target_rank + rank_step*step_size;
}

bool Threat_Iterator::is_a_threat() const
{
    auto piece = board.piece_on_square(attacking_file(), attacking_rank());
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
    // Iterator is initialized with step_size == 0, since this is
    // the square that we want to check if it's attacked, ignore any
    // piece already on it.
    if(step_size > 0 && way_blocked())
    {
        step_size = 8; // skip to next direction
    }

    ++step_size;

    for( ; file_step <= 2; ++file_step)
    {
        for( ; rank_step <= 2; ++rank_step)
        {
            // Filter invalid move steps, namely (0, 0) and (+/-2, +/-2)
            // for (file_change, rank_change).
            if(file_step == 0 && rank_step == 0)
            {
                continue;
            }

            // Only knight moves should have a file/rank step of more than 1
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
                // Knight moves have only one possible length of move, unlike
                // pieces that move in straight lines.
                if((std::abs(file_step) > 1 || std::abs(rank_step) > 1) && step_size > 1)
                {
                    break;
                }

                if( ! board.inside_board(attacking_file(), attacking_rank()))
                {
                    break; // go to next direction
                }

                if(is_a_threat())
                {
                    return; // threatening move will be returned by operator*()
                }

                if(way_blocked())
                {
                    break; // go to next direction
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
    target_file = '\0';
    target_rank = 0;
    file_step = 0;
    rank_step = 0;
    step_size = 0;
}

bool Threat_Iterator::way_blocked() const
{
    auto blocking_piece = board.piece_on_square(attacking_file(), attacking_rank());
    auto king = (const Piece*) board.get_king(opposite(attacking_color));
    return blocking_piece && blocking_piece != king;
}
