#ifndef PIECE_STRENGTH_GENE_H
#define PIECE_STRENGTH_GENE_H

#include "Gene.h"

#include <string>
#include <array>
#include <memory>
#include <map>

#include "Game/Piece_Types.h"

class Piece;
class Board;

//! Provides a database of the value of different piece types to be referenced by other Genes.
class Piece_Strength_Gene : public Gene
{
    public:
        Piece_Strength_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

        double piece_value(const Piece* piece) const;
        double normalizer() const;

    protected:
        std::map<std::string, double> list_properties() const override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        std::array<double, 6> piece_strength;
        double normalizing_value;

        double score_board(const Board& board, const Board& opposite_board, size_t depth) const override;
        double piece_value(Piece_Type type) const;
        double& piece_value(Piece_Type type);
        void gene_specific_mutation() override;
        void recalculate_normalizing_value();
};

#endif // PIECE_STRENGTH_GENE_H
