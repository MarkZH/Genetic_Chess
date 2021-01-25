#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on the number of squares attacked by a side with bonus points for legality and moves close to the king.
class Sphere_of_Influence_Gene : public Clonable_Gene<Sphere_of_Influence_Gene>
{
    public:
        Sphere_of_Influence_Gene() noexcept;

        std::string name() const noexcept override;

    private:
        double opening_legal_square_score = 1.0;
        double opening_illegal_square_score = 1.0;
        double opening_king_target_factor = 0.0;

        double endgame_legal_square_score = 1.0;
        double endgame_illegal_square_score = 1.0;
        double endgame_king_target_factor = 0.0;

        double score_board(const Board& board, Piece_Color perspective, const size_t depth, double game_progress) const noexcept override;
        void gene_specific_mutation() noexcept override;

        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
        void normalize_square_scores() noexcept;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
