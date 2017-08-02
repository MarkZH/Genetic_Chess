#ifndef PIECE_STRENGTH_GENE_H
#define PIECE_STRENGTH_GENE_H

#include "Gene.h"

#include <string>
#include <array>
#include <memory>

class Piece;
class Board;

// Database of the value of piece types
class Piece_Strength_Gene : public Gene
{
    public:
        Piece_Strength_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

        double piece_value(const Piece* piece) const;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        static const std::string piece_types;
        static const char first_piece;
        std::array<double, 'R' - 'B' + 1> piece_strength;
        double normalizing_factor;

        double score_board(const Board& board) const override;
        void renormalize();
        double piece_value(char symbol) const;
        double& piece_value(char symbol);
        void gene_specific_mutation() override;
};

#endif // PIECE_STRENGTH_GENE_H
