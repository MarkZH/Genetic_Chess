#include "Moves/Threat_Iterator.h"

#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Color.h"
#include "Game/Piece.h"
#include "Game/Piece_Types.h"
#include "Moves/Move.h"

Threat_Iterator::Threat_Iterator(char target_file_in,
                                 int  target_rank_in,
                                 Color attack_color,
                                 const Board& reference_board) :
    target_file(target_file_in),
    target_rank(target_rank_in),
    file_step(-1),
    rank_step(-2),
    step_size(0),
    knight_index(0),
    hit_count(0),
    max_hit_count(3),
    attacking_color(attack_color),
    board(reference_board),
    target_king(reference_board.piece_instance(KING, opposite(attack_color)))
{
    next_threat();
}

void Threat_Iterator::operator++()
{
    next_threat();
}

Square Threat_Iterator::operator*() const
{
    return {attacking_file(), attacking_rank()};
}

bool Threat_Iterator::operator==(const Threat_Iterator& other) const
{
    return hit_count == other.hit_count;
}

bool Threat_Iterator::operator!=(const Threat_Iterator& other) const
{
    return ! (*this == other);
}

char Threat_Iterator::attacking_file() const
{
    return attack_file;
}

int Threat_Iterator::attacking_rank() const
{
    return attack_rank;
}

void Threat_Iterator::next_threat()
{
    if(++hit_count == max_hit_count)
    {
        return;
    }

    ++rank_step;

    for( ; knight_index == 0 && file_step <= 1; ++file_step)
    {
        for( ; rank_step <= 1; ++rank_step)
        {
            // Filter non-moves
            if(file_step == 0 && rank_step == 0)
            {
                continue;
            }

            for(step_size = 1 ; step_size <= 7; ++step_size)
            {
                attack_file = target_file + file_step*step_size;
                attack_rank = target_rank + rank_step*step_size;

                if( ! board.inside_board(attack_file, attack_rank))
                {
                    break; // go to next direction
                }

                auto piece = board.piece_on_square(attack_file, attack_rank);
                if(( ! piece) || piece == target_king)
                {
                    continue;
                }

                if(piece->color() != attacking_color)
                {
                    break;
                }

                if(piece->type() == QUEEN)
                {
                    return;
                }

                if(piece->type() == KING && step_size == 1)
                {
                    return;
                }

                if(rank_step == 0 || file_step == 0)
                {
                    if(piece->type() == ROOK)
                    {
                        return;
                    }
                }
                else
                {
                    if(piece->type() == BISHOP)
                    {
                        return;
                    }

                    if(piece->type() == PAWN && step_size == 1 && rank_step == (attacking_color == WHITE ? -1 : 1))
                    {
                        return;
                    }
                }

                break; // Piece on square blocks farther movements
            }
        }

        rank_step = -1;
    }

    auto knight = board.piece_instance(KNIGHT, attacking_color);
    const auto& moves = knight->move_list(target_file, target_rank);
    while(knight_index < moves.size())
    {
        auto move = moves[knight_index];
        ++knight_index;
        attack_file = move->end_file();
        attack_rank = move->end_rank();
        if(board.piece_on_square(attack_file, attack_rank) == knight)
        {
            return;
        }
    }

    convert_to_end_iterator();
}

Threat_Iterator Threat_Iterator::make_end_iterator() const
{
    auto end = *this;
    end.convert_to_end_iterator();
    return end;
}

void Threat_Iterator::convert_to_end_iterator()
{
    hit_count = max_hit_count;
}
