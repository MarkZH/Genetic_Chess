#ifndef KING_CONFINEMENT_GENE_H
#define KING_CONFINEMENT_GENE_H

#include "Genes/Gene.h"


class King_Confinement_Gene : public Gene
{
    public:
        King_Confinement_Gene();
        virtual ~King_Confinement_Gene() override;

        double score_board(const Board& board, Color perspective) const override;

        King_Confinement_Gene* duplicate() const override;

        std::string name() const override;

    private:
        void reset_properties() const override;
        void load_properties() override;
};

#endif // KING_CONFINEMENT_GENE_H
