#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include <array>
#include <memory>
#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Genes/Gene.h"
#include "Game/Color.h"
#include "Genes/Piece_Strength_Gene.h"

//! The Opponent_Pieces_Targeted_Gene constructor requires a Piece_Strength_Gene to reference in score_board().

//! \param piece_strength_gene The source of piece values to weight the importance of various pieces.
Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) :
    piece_strength_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, Color perspective, size_t) const
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

    return score/piece_strength_source->normalizer();
}

std::unique_ptr<Gene> Opponent_Pieces_Targeted_Gene::duplicate() const
{
    return std::make_unique<Opponent_Pieces_Targeted_Gene>(*this);
}

std::string Opponent_Pieces_Targeted_Gene::name() const
{
    return "Opponent Pieces Targeted Gene";
}

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* psg)
{
    piece_strength_source = psg;
}
