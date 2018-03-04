#ifndef KING_CONFINEMENT_GENE_H
#define KING_CONFINEMENT_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <memory>

class Board;

// Counts squares accessible to king (escape routes)
class King_Confinement_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;
        void gene_specific_mutation() override;

        std::string name() const override;

    protected:
        void load_properties() override;
        void reset_properties() const override;

    private:
        double friendly_block_score;
        double enemy_block_score;
        double free_square_score;

        double score_board(const Board& board) const override;
};

#endif // KING_CONFINEMENT_GENE_H
