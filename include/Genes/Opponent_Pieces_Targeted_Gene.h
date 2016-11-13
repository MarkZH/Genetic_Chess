#ifndef OPPONENT_PIECES_TARGETED_GENE_H
#define OPPONENT_PIECES_TARGETED_GENE_H

#include <memory>

#include "Gene.h"
#include "Piece_Strength_Gene.h"

class Opponent_Pieces_Targeted_Gene : public Gene
{
    public:
        explicit Opponent_Pieces_Targeted_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_gene);
        virtual ~Opponent_Pieces_Targeted_Gene() override;

        Opponent_Pieces_Targeted_Gene* duplicate() const override;
        void reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg) override;

        std::string name() const override;

    private:
        std::shared_ptr<const Piece_Strength_Gene> piece_strenth_source;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // OPPONENT_PIECES_TARGETED_GENE_H
