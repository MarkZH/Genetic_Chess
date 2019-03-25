#ifndef KING_CONFINEMENT_GENE_H
#define KING_CONFINEMENT_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <memory>
#include <map>

class Board;

//! Scores the board based on how confined the king is by friendly pieces or opponent attacks.
class King_Confinement_Gene : public Gene
{
    public:
        King_Confinement_Gene();
        std::unique_ptr<Gene> duplicate() const override;
        void gene_specific_mutation() override;

        std::string name() const override;

    protected:
        void load_properties(const std::map<std::string, double>& properties) override;
        std::map<std::string, double> list_properties() const override;

    private:
        double friendly_block_score;
        double enemy_block_score;
        double maximum_square_count;

        double score_board(const Board& board, const Board& opposite_board, size_t depth) const override;
};

#endif // KING_CONFINEMENT_GENE_H
