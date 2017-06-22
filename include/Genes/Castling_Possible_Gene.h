#ifndef CASTLING_POSSIBLE_GENE_H
#define CASTLING_POSSIBLE_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

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

        double score_board(const Board& board, Color perspective) const override;
        void gene_specific_mutation() override;
};

#endif // CASTLING_POSSIBLE_GENE_H
