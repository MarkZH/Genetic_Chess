#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

class Board;

// How close are pawns to promotion?
class Pawn_Advancement_Gene : public Gene
{
    public:
        Pawn_Advancement_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double non_linearity;

        double score_board(const Board& board, const Board& opposite_board, size_t) const override;
        void gene_specific_mutation() override;
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
