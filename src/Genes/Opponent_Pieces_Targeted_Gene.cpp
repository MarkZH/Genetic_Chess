#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* const piece_strength_gene) noexcept :
    piece_strength_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, const Piece_Color perspective, size_t, double) const noexcept
{
    assert(piece_strength_source);
    double score = 0.0;
    const auto values = piece_strength_source->piece_values();

    for(const auto square : Square::all_squares())
    {
        const auto piece = board.piece_on_square(square);
        if(piece && piece.color() != perspective)
        {
            if( ! board.safe_for_king(square, opposite(perspective)))
            {
                score += values[static_cast<size_t>(piece.type())];
            }
        }
    }

    return score;
}

std::string Opponent_Pieces_Targeted_Gene::name() const noexcept
{
    return "Opponent Pieces Targeted Gene";
}

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* const psg) noexcept
{
    piece_strength_source = psg;
}
