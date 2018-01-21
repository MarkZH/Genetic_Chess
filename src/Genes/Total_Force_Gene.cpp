#include "Genes/Total_Force_Gene.h"

#include <memory>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Total_Force_Gene::Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source_in) :
    piece_strength_source(piece_strength_source_in)
{
}

double Total_Force_Gene::score_board(const Board& board) const
{
    double score = 0.0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece && piece->color() == board.whose_turn())
            {
                score += piece_strength_source->piece_value(piece);
            }
        }
    }

    return score;
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
