#include "Genes/King_Confinement_Gene.h"

#include <array>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Game/Piece.h"

std::string King_Confinement_Gene::name() const noexcept
{
    return "King Confinement Gene";
}

double King_Confinement_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    std::array<Square, 64> square_queue{};
    size_t queue_insertion_point = 0;
    std::array<bool, 64> in_queue{};

    const auto king_square = board.find_king(perspective);

    square_queue[queue_insertion_point++] = king_square;
    in_queue[king_square.index()] = true;

    double free_space_total = 0.0;

    for(auto square : square_queue)
    {
        if( ! square.is_set())
        {
            break;
        }

        // Always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check).
        auto add_surrounding_squares = square == king_square;

        auto piece = board.piece_on_square(square);
        auto blocked_by_friendly = piece && piece.color() == perspective && piece.type() != Piece_Type::KING;
        auto attacked_by_opposing =  ! board.safe_for_king(square, perspective);

        if( ! blocked_by_friendly && ! attacked_by_opposing)
        {
            free_space_total += 1.0;
            add_surrounding_squares = true;
        }

        // Add surrounding squares to square_queue.
        if(add_surrounding_squares)
        {
            for(auto file_step = -1; file_step <= 1; ++file_step)
            {
                for(auto rank_step = -1; rank_step <= 1; ++rank_step)
                {
                    auto new_square = square + Square_Difference{file_step, rank_step};
                    if(new_square.inside_board() && ! in_queue[new_square.index()])
                    {
                        square_queue[queue_insertion_point++] = new_square;
                        in_queue[new_square.index()] = true;
                    }
                }
            }
        }
    }

    return free_space_total/64;
}
