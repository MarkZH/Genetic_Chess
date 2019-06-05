#include "Genes/King_Protection_Gene.h"

#include <memory>
#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"

double King_Protection_Gene::score_board(const Board& board, Color perspective, size_t) const
{
    // Count the number of empty squares from which a piece could attack the king. This
    // is a measure of the exposure of the king

    const auto& king_square = board.find_king(perspective);

    auto square_count = 0;

    // Rows, columns, diagonals
    for(auto file_step : {-1, 0, 1})
    {
        for(auto rank_step : {-1, 0, 1})
        {
            if(file_step == 0 && rank_step == 0)
            {
                continue;
            }

            auto step = Square_Difference{file_step, rank_step};
            for(auto square : Square::square_line_from(king_square, step))
            {
                if(board.piece_on_square(square))
                {
                    break;
                }
                else
                {
                    ++square_count;
                }
            }
        }
    }

    // Knight moves
    for(auto file_step : {1, 2})
    {
        auto rank_step  = 3 - file_step; // (1,2) or (2,1) move

        for(auto file_direction : {-1, 1})
        {
            for(auto rank_direction : {-1, 1})
            {
                auto square = king_square + Square_Difference{file_direction*file_step,
                                                              rank_direction*rank_step};

                if( ! square.inside_board())
                {
                    continue;
                }

                if( ! board.piece_on_square(square))
                {
                    ++square_count;
                }
            }
        }
    }

    constexpr int max_square_count =  8      // knight attack
                                    + 7 + 7  // rooks/queen row/column attack
                                    + 7 + 6; // bishop/queen/pawn attack
    return double(max_square_count - square_count)/max_square_count; // return score [0, 1]
}

std::unique_ptr<Gene> King_Protection_Gene::duplicate() const
{
    return std::make_unique<King_Protection_Gene>(*this);
}

std::string King_Protection_Gene::name() const
{
    return "King Protection Gene";
}
