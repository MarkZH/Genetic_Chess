#ifndef CASTLING_POSSIBLE_GENE_H
#define CASTLING_POSSIBLE_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"

class Board;

//! \brief This gene scores a board based of if castling is possible or has already happened.
class Castling_Possible_Gene : public Clonable_Gene<Castling_Possible_Gene>
{
    public:
        Castling_Possible_Gene() noexcept;

        std::string name() const noexcept override;

    private:
        double opening_kingside_preference = 1.0;
        double opening_queenside_preference = 1.0;
        double endgame_kingside_preference = 1.0;
        double endgame_queenside_preference = 1.0;
        double opening_rook_unmoved_score = 0.0;
        double endgame_rook_unmoved_score = 0.0;

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
        void gene_specific_mutation() noexcept override;
        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
        void normalize_sides() noexcept;
};

#endif // CASTLING_POSSIBLE_GENE_H
