#ifndef PIECE_STRENGTH_GENE_H
#define PIECE_STRENGTH_GENE_H

#include "Gene.h"
class Piece;


class Piece_Strength_Gene : public Gene
{
    public:
        Piece_Strength_Gene();
        ~Piece_Strength_Gene() override;

        void mutate() override;
        Piece_Strength_Gene* duplicate() const override;

        std::string name() const override;

        double piece_value(const std::shared_ptr<const Piece>& piece) const;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        std::map<char, double> piece_strength;
        double normalizing_factor;

        double score_board(const Board& board, Color perspective) const override;
        void renormalize();
        double piece_value(char symbol) const;
};

#endif // PIECE_STRENGTH_GENE_H
