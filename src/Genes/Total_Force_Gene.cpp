#include "Genes/Total_Force_Gene.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"
#include "Genes/Gene.h"

Total_Force_Gene::Total_Force_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_source_in) :
    Gene(1.0),
    piece_strength_source(piece_strength_source_in)
{
}

void Total_Force_Gene::reset_properties() const
{
    reset_base_properties();
}

void Total_Force_Gene::load_properties()
{
    load_base_properties();
}

double Total_Force_Gene::score_board(const Board& board, Color color) const
{
    double score = 0.0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece && piece->color() == color)
            {
                score += piece_strength_source->piece_value(piece);
            }
        }
    }

    return score;
}

Total_Force_Gene* Total_Force_Gene::duplicate() const
{
    return new Total_Force_Gene(*this);
}

std::string Total_Force_Gene::name() const
{
    return "Total Force Gene";
}

void Total_Force_Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg)
{
    piece_strength_source = psg;
}
