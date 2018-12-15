#ifndef PRIORITY_THRESHOLD_GENE_H
#define PRIORITY_THRESHOLD_GENE_H

#include <memory>
#include <string>

#include "Gene.h"

class Board;

class Priority_Threshold_Gene : public Gene
{
    public:
        Priority_Threshold_Gene();

        std::unique_ptr<Gene> duplicate() const override;
        void gene_specific_mutation() override;
        std::string name() const override;
        double score_board(const Board& board) const override;

        double get_threshold() const;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double threshold;
};

#endif // PRIORITY_THRESHOLD_GENE_H
