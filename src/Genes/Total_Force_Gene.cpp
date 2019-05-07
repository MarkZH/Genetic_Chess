#include "Genes/Total_Force_Gene.h"

#include <memory>
#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Genes/Gene.h"
#include "Game/Color.h"
#include "Genes/Piece_Strength_Gene.h"

//! The Total_Force_Gene constructor requires a Piece_Strength_Gene to reference in score_board().
//
//! \param piece_strength_source_in The source of piece values to weight the importance of various pieces.
Total_Force_Gene::Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source_in) :
    Gene(true),
    piece_strength_source(piece_strength_source_in)
{
}

double Total_Force_Gene::score_board(const Board& board, Color perspective, size_t) const
{
    double score = 0.0;

    for(auto square : Square::all_squares())
    {
        auto piece = board.piece_on_square(square);
        if(piece && piece.color() == perspective)
        {
            score += piece_strength_source->piece_value(piece);
        }
    }

    return score/piece_strength_source->normalizer();
}

std::unique_ptr<Gene> Total_Force_Gene::duplicate() const
{
    return std::make_unique<Total_Force_Gene>(*this);
}

std::string Total_Force_Gene::name() const
{
    return "Total Force Gene";
}

void Total_Force_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* psg)
{
    piece_strength_source = psg;
}
