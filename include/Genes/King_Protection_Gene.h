#ifndef KING_PROTECTION_GENE_H
#define KING_PROTECTION_GENE_H

#include "Genes/Gene.h"


class King_Protection_Gene : public Gene
{
    public:
        King_Protection_Gene();
        virtual ~King_Protection_Gene() override;

        King_Protection_Gene* duplicate() const override;

        std::string name() const override;

    private:
        void reset_properties() const override;
        void load_properties() override;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // KING_PROTECTION_GENE_H
