#ifndef CASTLING_POSSIBLE_GENE_H
#define CASTLING_POSSIBLE_GENE_H

#include "Gene.h"

#include <string>
#include <array>

class Board;

// Adds points to board if castling is possible or already happened
class Castling_Possible_Gene : public Gene
{
    public:
        Castling_Possible_Gene();

        std::string name() const override;
        Castling_Possible_Gene* duplicate() const override;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double kingside_preference;

        mutable std::array<double, 2> permanent_value; // value to return once castling
                                                       // has occurred or is impossible

        double score_board(const Board& board) const override;
        void gene_specific_mutation() override;
};

#endif // CASTLING_POSSIBLE_GENE_H
