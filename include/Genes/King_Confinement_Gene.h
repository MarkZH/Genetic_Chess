#ifndef KING_CONFINEMENT_GENE_H
#define KING_CONFINEMENT_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"

class Board;

//! \brief Scores the board based on how confined the king is by friendly pieces or opponent attacks.
//!
//! This gene uses a flood-fill-like algorithm to count the squares that are reachable by the
//! king from its current positions with unlimited consecutive moves. The boundaries of this
//! area are squares attacked by the other player or occupied by pieces of the same color.
class King_Confinement_Gene : public Clonable_Gene<King_Confinement_Gene>
{
    public:
        King_Confinement_Gene() noexcept;

        std::string name() const noexcept override;

    private:
        double friendly_block_score = 1.0;
        double opponent_block_score = -1.0;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
        void gene_specific_mutation() noexcept override;
};

#endif // KING_CONFINEMENT_GENE_H
