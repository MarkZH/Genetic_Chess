#ifndef OPPONENT_PIECES_TARGETED_GENE_H
#define OPPONENT_PIECES_TARGETED_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

class Piece_Strength_Gene;
class Board;

// Weighted count of how many opponent's pieces are threatened
class Opponent_Pieces_Targeted_Gene : public Gene
{
    public:
        explicit Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene);

        Opponent_Pieces_Targeted_Gene* duplicate() const override;
        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) override;

        std::string name() const override;

    private:
        const Piece_Strength_Gene* piece_strenth_source;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // OPPONENT_PIECES_TARGETED_GENE_H
