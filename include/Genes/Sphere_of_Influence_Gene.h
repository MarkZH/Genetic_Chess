#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"
#include "Gene_Value.h"

class Board;

//! \brief Scores a board based on the number of squares attacked by a side with bonus points for legality and moves close to the king.
class Sphere_of_Influence_Gene : public Clonable_Gene<Sphere_of_Influence_Gene>
{
    public:
        Sphere_of_Influence_Gene() noexcept;

    private:
        Gene_Value legal_square_score = {"Legal Square Score", 1.0, 0.03};
        Gene_Value illegal_square_score = {"Illegal Square Score", 1.0, 0.03};

        double score_board(const Board& board, Piece_Color perspective, const size_t depth) const noexcept override;
        void gene_specific_mutation() noexcept override;

        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;
        void normalize_square_scores() noexcept;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
