#ifndef CASTLING_POSSIBLE_GENE_H
#define CASTLING_POSSIBLE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>
#include <map>

class Board;

//! This gene scores a board based of if castling is possible or has already happened.
class Castling_Possible_Gene : public Gene
{
    public:
        Castling_Possible_Gene();

        std::string name() const override;
        std::unique_ptr<Gene> duplicate() const override;

    protected:
        std::map<std::string, double> list_properties() const override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double kingside_preference;
        double queenside_preference;

        double score_board(const Board& board, const Board& opposite_board, size_t depth) const override;
        void gene_specific_mutation() override;
};

#endif // CASTLING_POSSIBLE_GENE_H
