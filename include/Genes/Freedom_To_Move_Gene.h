#ifndef FREEDOM_TO_MOVE_GENE_H
#define FREEDOM_TO_MOVE_GENE_H

#include <fstream>

#include "Gene.h"

// Total number of legal moves
class Freedom_To_Move_Gene : public Gene
{
    public:
        Freedom_To_Move_Gene();
        explicit Freedom_To_Move_Gene(std::ifstream& ifs);
        ~Freedom_To_Move_Gene() override;

        Freedom_To_Move_Gene* duplicate() const override;

        std::string name() const override;

    private:
        size_t maximum_number_of_moves;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // FREEDOM_TO_MOVE_GENE_H
