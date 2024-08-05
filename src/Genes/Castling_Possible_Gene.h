#ifndef CASTLING_POSSIBLE_GENE_H
#define CASTLING_POSSIBLE_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"
#include "Gene_Value.h"

class Board;

//! \brief Scores a board based on if castling is possible or has already happened.
class Castling_Possible_Gene : public Clonable_Gene<Castling_Possible_Gene>
{
    public:
        Castling_Possible_Gene() noexcept;

    private:
        Gene_Value kingside_preference = {"Kingside Preference", 1.0, 0.02};
        Gene_Value queenside_preference = {"Queenside Preference", 1.0, 0.02};

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
        void gene_specific_mutation() noexcept override;
        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;
        void normalize_sides() noexcept;
};

#endif // CASTLING_POSSIBLE_GENE_H
