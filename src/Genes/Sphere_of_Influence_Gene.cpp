#include "Genes/Sphere_of_Influence_Gene.h"

#include "Genes/Gene.h"

#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Color.h"

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() noexcept : Clonable_Gene("Sphere of Influence Gene")
{
}

double Sphere_of_Influence_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    double score = 0.0;
    for(const auto square : Square::all_squares())
    {
        if(board.attacked_by(square, perspective)) // any piece attacks square
        {
            const auto invasion_distance = (perspective == Piece_Color::WHITE ? square.rank() : 9 - square.rank());
            score += invasion_distance;
        }
    }

    return score/288;
}
