#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"


class Pawn_Advancement_Gene : public Gene
{
    public:
        explicit Pawn_Advancement_Gene();

        Pawn_Advancement_Gene* duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board, Color perspective) const override;
        void reset_properties() const override;
        void load_properties() override;
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
