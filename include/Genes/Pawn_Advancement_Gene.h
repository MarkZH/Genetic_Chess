#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"


class Pawn_Advancement_Gene : public Gene
{
    public:
        ~Pawn_Advancement_Gene() override;

        Pawn_Advancement_Gene* duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board, Color perspective) const override;
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
