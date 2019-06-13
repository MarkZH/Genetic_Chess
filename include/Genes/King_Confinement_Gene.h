#ifndef KING_CONFINEMENT_GENE_H
#define KING_CONFINEMENT_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <memory>
#include <map>

#include "Game/Color.h"

class Board;

//! Scores the board based on how confined the king is by friendly pieces or opponent attacks.
//
//! This gene uses a flood-fill-like algorithm to count the squares that are reachable by the
//! king from its current positions with unlimited consecutive moves. The boundaries of this
//! area are squares attacked by the other player or occupied by pieces of the same color.
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
        double opponent_block_score;

        double score_board(const Board& board, Color perspective, size_t depth) const override;
};

#endif // KING_CONFINEMENT_GENE_H
