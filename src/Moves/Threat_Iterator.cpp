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
    rank_step(-1),
    step_size(0),
    knight_index(-1),
    on_knight_moves(false),
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

bool Threat_Iterator::is_a_threat() const
{
    auto piece = board.piece_on_square(attacking_file(), attacking_rank());
    if(( ! piece) || piece->color() != attacking_color)
    {
        return false;
    }

    return piece->can_attack(step_size, file_step, rank_step);
}

void Threat_Iterator::next_threat()
{
    if(on_knight_moves)
    {
        goto knight_continuation_point;
    }

    // Iterator is initialized with step_size == 0. So, after the
    // first run-through, skip to just after the return statement to
    // resume where we left off.
    if(step_size > 0)
    {
        goto resume_point;
    }
    else
    {
        step_size = 1;
    }

    for( ; file_step <= 1; ++file_step)
    {
        for( ; rank_step <= 1; ++rank_step)
        {
            // Filter invalid move steps, namely (0, 0) and (+/-2, +/-2)
            // for (file_change, rank_change).
            if(file_step == 0 && rank_step == 0)
            {
                continue;
            }

            for( ; step_size <= 7; ++step_size)
            {
                if( ! board.inside_board(attacking_file(), attacking_rank()))
                {
                    break; // go to next direction
                }

                if(is_a_threat())
                {
                    return; // threatening move will be returned by operator*()
                }

                resume_point:
                if(way_blocked())
                {
                    break; // go to next direction
                }
            }

            step_size = 1;
        }

        rank_step = -1;
    }

    on_knight_moves = true;
    step_size = 1;

    knight_continuation_point:
    auto knight = board.get_knight(attacking_color);
    const auto& moves = knight->get_move_list(target_file, target_rank);
    while(++knight_index < moves.size())
    {
        auto move = moves[knight_index];
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
}

bool Threat_Iterator::way_blocked() const
{
    // Threats are only relevant to moves that attack the king. So, in case
    // the square being checked for attack is a square the king used to
    // occupy, pretend the king is not there so you don't have the king
    // blocking itself.
    auto blocking_piece = board.piece_on_square(attacking_file(), attacking_rank());
    auto king = (const Piece*) board.get_king(opposite(attacking_color));
    return blocking_piece && blocking_piece != king;
}
