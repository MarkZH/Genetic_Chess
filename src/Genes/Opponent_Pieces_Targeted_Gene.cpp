#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) noexcept :
    piece_strength_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, Piece_Color perspective, size_t, double) const noexcept
{
    double score = 0.0;

    for(auto square : Square::all_squares())
    {
        auto piece = board.piece_on_square(square);
        if(piece && piece.color() != perspective)
        {
            if( ! board.safe_for_king(square, opposite(perspective)))
            {
                score += piece_strength_source->piece_value(piece);
            }
        }
    }

    return score;
}

std::string Opponent_Pieces_Targeted_Gene::name() const noexcept
{
    return "Opponent Pieces Targeted Gene";
}

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept
{
    piece_strength_source = psg;
}
