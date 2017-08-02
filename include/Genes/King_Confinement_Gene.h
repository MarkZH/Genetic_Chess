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
        King_Confinement_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        double maximum_score;

        double score_board(const Board& board) const override;
};

#endif // KING_CONFINEMENT_GENE_H
