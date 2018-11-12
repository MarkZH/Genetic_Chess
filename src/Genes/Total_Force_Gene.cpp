#include "Genes/Total_Force_Gene.h"

#include <memory>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Total_Force_Gene::Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source_in) :
    piece_strength_source(piece_strength_source_in)
{
}

double Total_Force_Gene::score_board(const Board& board, const Board&, size_t) const
{
    double score = 0.0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece &&
               piece->color() == board.whose_turn() &&
               piece->type() != KING)
            {
                score += piece_strength_source->piece_value(piece);
            }
        }
    }

    auto normalizer = 8*std::abs(piece_strength_source->piece_value(Board::piece_instance(PAWN, WHITE))) +
                      2*std::abs(piece_strength_source->piece_value(Board::piece_instance(ROOK, WHITE))) +
                      2*std::abs(piece_strength_source->piece_value(Board::piece_instance(KNIGHT, WHITE))) +
                      2*std::abs(piece_strength_source->piece_value(Board::piece_instance(BISHOP, WHITE))) +
                      1*std::abs(piece_strength_source->piece_value(Board::piece_instance(QUEEN, WHITE)));

    return score/normalizer;
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

void Total_Force_Gene::gene_specific_mutation()
{
    make_priority_minimum_zero();
}
