#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"


class Pawn_Advancement_Gene : public Gene
{
    public:
        explicit Pawn_Advancement_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_source_in);

        Pawn_Advancement_Gene* duplicate() const override;
        void mutate() override;

        std::string name() const override;

        virtual void reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg) override;

    private:
        double score_board(const Board& board, Color color) const override;
        void reset_properties() const override;
        void load_properties() override;

        std::shared_ptr<const Piece_Strength_Gene> piece_strength_source;

        double promoted_pawn_bonus;
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
