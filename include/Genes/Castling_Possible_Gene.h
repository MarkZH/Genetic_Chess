#ifndef CASTLING_POSSIBLE_GENE_H
#define CASTLING_POSSIBLE_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"
#include "Interpolated_Gene_Value.h"

class Board;

//! \brief Scores a board based on if castling is possible or has already happened.
class Castling_Possible_Gene : public Clonable_Gene<Castling_Possible_Gene>
{
    public:
        Castling_Possible_Gene() noexcept;

        std::string name() const noexcept override;

    private:
        Interpolated_Gene_Value kingside_preferences = {"Kingside Preference", 1.0, 1.0, 0.03};
        Interpolated_Gene_Value queenside_preferences = {"Queenside Preference", 1.0, 1.0, 0.03};

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
        void gene_specific_mutation() noexcept override;
        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;
        void normalize_sides() noexcept;
};

#endif // CASTLING_POSSIBLE_GENE_H
