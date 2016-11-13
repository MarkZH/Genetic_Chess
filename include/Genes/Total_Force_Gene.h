#ifndef TOTAL_FORCE_GENE_H
#define TOTAL_FORCE_GENE_H

#include <memory>

#include "Gene.h"
#include "Piece_Strength_Gene.h"

// Weighted sum of all pieces of a certain color
class Total_Force_Gene : public Gene
{
    public:
        explicit Total_Force_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_source);
        virtual ~Total_Force_Gene() override;

        Total_Force_Gene* duplicate() const;
        void reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg) override;

        std::string name() const override;

    private:
        std::shared_ptr<const Piece_Strength_Gene> piece_strength_source;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // TOTAL_FORCE_GENE_H
