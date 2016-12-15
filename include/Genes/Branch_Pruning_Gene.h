#ifndef GENES_BRANCH_PRUNING_GENE_H
#define GENES_BRANCH_PRUNING_GENE_H

#include "Gene.h"

class Board;

class Branch_Pruning_Gene : public Gene
{
    public:
        Branch_Pruning_Gene();
        virtual ~Branch_Pruning_Gene();

        Branch_Pruning_Gene* duplicate() const override;
        void mutate() override;

        std::string name() const override;

        bool good_enough_to_examine(double score_difference) const;

    protected:
        void reset_properties() const override;
        void load_properties() override;


    private:
        double minimum_score_change;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // GENES_BRANCH_PRUNING_GENE_H
