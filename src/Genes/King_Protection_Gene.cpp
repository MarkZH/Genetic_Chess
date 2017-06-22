#include "Genes/King_Protection_Gene.h"

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

double King_Protection_Gene::score_board(const Board& board, Color perspective) const
{
    // Count the number of empty squares from which a piece could attack the king. This
    // is a measure of the exposure of the king

    auto king_square = board.find_king(perspective);

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

            for(int steps = 1; steps <= 7; ++steps)
            {
                char file = king_square.file + steps*file_step;
                int  rank = king_square.rank + steps*rank_step;

                if(! board.inside_board(file, rank))
                {
                    break;
                }

                if(board.view_piece_on_square(file, rank))
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
                char file = king_square.file + file_direction*file_step;
                int  rank = king_square.rank + rank_direction*rank_step;

                if( ! board.inside_board(file, rank))
                {
                    continue;
                }

                if( ! board.view_piece_on_square(file, rank))
                {
                    ++square_count;
                }
            }
        }
    }

    const int max_square_count =  8      // knight attack
                                + 7 + 7  // rooks/queen row/column attack
                                + 7 + 6; // bishop/queen/pawn attack
    return double(max_square_count - square_count)/max_square_count; // return score [0, 1]
}

King_Protection_Gene* King_Protection_Gene::duplicate() const
{
    return new King_Protection_Gene(*this);
}

std::string King_Protection_Gene::name() const
{
    return "King Protection Gene";
}
