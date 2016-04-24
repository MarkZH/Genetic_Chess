#ifndef PIECE_STRENGTH_GENE_H
#define PIECE_STRENGTH_GENE_H

#include "Gene.h"
class Piece;


class Piece_Strength_Gene : public Gene
{
    public:
        explicit Piece_Strength_Gene();
        ~Piece_Strength_Gene();

        void mutate() override;
        Piece_Strength_Gene* duplicate() const override;

        std::string name() const override;

        double piece_value(const std::shared_ptr<const Piece>& piece) const;
        double piece_value(char symbol) const;

    private:
        std::map<char, double> piece_strength;

        double score_board(const Board& board, Color color) const override;
        void reset_properties() override;
};

#endif // PIECE_STRENGTH_GENE_H
