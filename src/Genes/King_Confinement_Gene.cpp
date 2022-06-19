#include "Genes/King_Confinement_Gene.h"

#include <array>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Game/Piece.h"

#include "Utility/Fixed_Capacity_Vector.h"

namespace
{
    void add_surrounding_squares(const Square square,
                                 Fixed_Capacity_Vector<Square, 64>& square_queue,
                                 std::array<bool, 64>& in_queue) noexcept
    {
        for(auto file_step = -1; file_step <= 1; ++file_step)
        {
            for(auto rank_step = -1; rank_step <= 1; ++rank_step)
            {
                const auto new_square = square + Square_Difference{file_step, rank_step};
                if(new_square.inside_board() && ! in_queue[new_square.index()])
                {
                    square_queue.push_back(new_square);
                    in_queue[new_square.index()] = true;
                }
            }
        }
    }
}

King_Confinement_Gene::King_Confinement_Gene() noexcept : Clonable_Gene("King Confinement Gene")
{
}

double King_Confinement_Gene::score_board(const Board& board, const Piece_Color perspective, size_t) const noexcept
{
    Fixed_Capacity_Vector<Square, 64> square_queue{};
    std::array<bool, 64> in_queue{};

    const auto king_square = board.find_king(perspective);
    in_queue[king_square.index()] = true;

    // Always check the squares surrounding the king's current positions, even if
    // it is not safe (i.e., the king is in check).
    add_surrounding_squares(king_square, square_queue, in_queue);

    double free_space_total = board.safe_for_king(king_square, perspective) ? 1.0 : 0.0;

    // This raw for-loop is needed because add_surrounding_squares() modifies
    // square_queue.
    for(auto iter = square_queue.begin(); iter != square_queue.end(); ++iter)
    {
        const auto square = *iter;
        if( ! board.piece_on_square(square) && board.safe_for_king(square, perspective))
        {
            free_space_total += 1.0;
            add_surrounding_squares(square, square_queue, in_queue);
        }
    }

    return free_space_total/64;
}
