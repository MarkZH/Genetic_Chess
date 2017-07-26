#ifndef TOTAL_FORCE_GENE_H
#define TOTAL_FORCE_GENE_H

#include "Gene.h"

#include <string>

class Board;
class Piece_Strength_Gene;

// Weighted sum of all pieces of a certain color
class Total_Force_Gene : public Gene
{
    public:
        explicit Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source);

        Total_Force_Gene* duplicate() const;
        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) override;

        std::string name() const override;

    private:
        const Piece_Strength_Gene* piece_strength_source;

        double score_board(const Board& board) const override;
};

#endif // TOTAL_FORCE_GENE_H
