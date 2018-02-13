#include "Moves/Threat_Iterator.h"

#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"
#include "Pieces/Knight.h"

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
    attacking_color(attack_color),
    board(reference_board)
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
    return target_file == other.target_file &&
           target_rank == other.target_rank &&
           file_step == other.file_step &&
           rank_step == other.rank_step &&
           step_size == other.step_size &&
           knight_index == other.knight_index &&
           attacking_color == other.attacking_color &&
           &board == &other.board;
}

bool Threat_Iterator::operator!=(const Threat_Iterator& other) const
{
    return ! (*this == other);
}

char Threat_Iterator::attacking_file() const
{
    return target_file + file_step*step_size;
}

int Threat_Iterator::attacking_rank() const
{
    return target_rank + rank_step*step_size;
}

void Threat_Iterator::next_threat()
{
    ++rank_step;

    for( ; knight_index == 0 && file_step <= 1; ++file_step)
    {
        if( ! board.inside_board(attacking_file()))
        {
            continue;
        }

        for( ; rank_step <= 1; ++rank_step)
        {
            // Filter non-moves
            if(file_step == 0 && rank_step == 0)
            {
                continue;
            }

            if( ! board.inside_board(attacking_rank()))
            {
                continue;
            }

            for(step_size = 1 ; step_size <= 7; ++step_size)
            {
                if( ! board.inside_board(attacking_file(), attacking_rank()))
                {
                    break; // go to next direction
                }

                auto piece = board.piece_on_square(attacking_file(), attacking_rank());
                if(( ! piece) || piece == (const Piece*)board.get_king(opposite(attacking_color)))
                {
                    continue;
                }

                if(piece->color() != attacking_color)
                {
                    break;
                }

                if(piece->is_queen())
                {
                    return;
                }

                if(piece->is_king() && step_size == 1)
                {
                    return;
                }

                if(rank_step == 0 || file_step == 0)
                {
                    if(piece->is_rook())
                    {
                        return;
                    }
                }
                else
                {
                    if(piece->is_bishop())
                    {
                        return;
                    }

                    if(piece->is_pawn() && step_size == 1 && rank_step == (attacking_color == WHITE ? -1 : 1))
                    {
                        return;
                    }
                }

                break; // Piece on square blocks farther movements
            }
        }

        rank_step = -1;
    }

    auto knight = board.get_knight(attacking_color);
    const auto& moves = knight->get_move_list(target_file, target_rank);
    while(knight_index < moves.size())
    {
        auto move = moves[knight_index];
        ++knight_index;
        if(board.piece_on_square(move->end_file(), move->end_rank()) == knight)
        {
            file_step = -move->file_change();
            rank_step = -move->rank_change();
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
    target_file = '\0';
    target_rank = 0;
    file_step = 0;
    rank_step = 0;
    step_size = 0;
    knight_index = -1;
    attacking_color = NONE;
}
