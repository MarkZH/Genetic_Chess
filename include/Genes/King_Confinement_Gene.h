#ifndef KING_CONFINEMENT_GENE_H
#define KING_CONFINEMENT_GENE_H

#include "Genes/Gene.h"


class King_Confinement_Gene : public Gene
{
    public:
        King_Confinement_Gene();
        ~King_Confinement_Gene() override;

        King_Confinement_Gene* duplicate() const override;

        std::string name() const override;

    private:
        double maximum_score;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // KING_CONFINEMENT_GENE_H
