#include "Genes/King_Protection_Gene.h"

#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Moves/Move.h"

double King_Protection_Gene::score_board(const Board& board, const Piece_Color perspective, size_t, double) const noexcept
{
    auto square_count = 0;
    const auto king_square = board.find_king(perspective);

    for(size_t attack_index = 0; attack_index < 16; ++attack_index)
    {
        const auto step = Move::attack_direction_from_index(attack_index);
        for(const auto square : Square::square_line_from(king_square, step))
        {
            if(board.piece_on_square(square))
            {
                break;
            }
            else
            {
                ++square_count;
            }

            if(attack_index >= 8) // knight move
            {
                break;
            }
        }
    }

    constexpr int max_square_count =  8      // knight attack
                                    + 7 + 7  // rooks/queen row/column attack
                                    + 7 + 6; // bishop/queen/pawn attack
    return double(max_square_count - square_count)/max_square_count; // return score [0, 1]
}

std::string King_Protection_Gene::name() const noexcept
{
    return "King Protection Gene";
}
