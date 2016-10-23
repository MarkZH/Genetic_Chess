#ifndef BRANCH_PRUNING_GENE_H
#define BRANCH_PRUNING_GENE_H

#include "Genes/Gene.h"

class Branch_Pruning_Gene : public Gene
{
    public:
        Branch_Pruning_Gene();
        virtual ~Branch_Pruning_Gene();

        Branch_Pruning_Gene* duplicate() const override;
        void mutate() override;

        std::string name() const override;

        double minimum_score_change() const;
        double score_board(const Board& board, Color perspective) const override;

    private:
        double minimum_score_change_limit;

        void reset_properties() const override;
        void load_properties() override;
};

#endif // BRANCH_PRUNING_GENE_H
