#ifndef CASTLING_POSSIBLE_GENE_H
#define CASTLING_POSSIBLE_GENE_H

#include "Gene.h"

class Board;
enum Color;

class Castling_Possible_Gene : public Gene
{
    public:
        Castling_Possible_Gene();
        ~Castling_Possible_Gene() override;

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
