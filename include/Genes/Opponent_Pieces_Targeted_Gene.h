#ifndef OPPONENT_PIECES_TARGETED_GENE_H
#define OPPONENT_PIECES_TARGETED_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

class Piece_Strength_Gene;
class Board;

// Weighted count of how many opponent's pieces are threatened
class Opponent_Pieces_Targeted_Gene : public Gene
{
    public:
        explicit Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene);

        std::unique_ptr<Gene> duplicate() const override;
        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) override;

        std::string name() const override;

    private:
        const Piece_Strength_Gene* piece_strength_source;
        double normalizer;

        double score_board(const Board& board, const Board& opposite_board, size_t depth) const override;
        void recalculate_normalizer();
};

#endif // OPPONENT_PIECES_TARGETED_GENE_H
