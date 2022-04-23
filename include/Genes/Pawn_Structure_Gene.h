#ifndef PAWN_STRUCTURE_GENE_H
#define PAWN_STRUCTURE_GENE_H

#include <string>
#include <map>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Gene_Value.h"

class Board;

//! \brief A gene to evaluate how well pawns are protected.
class Pawn_Structure_Gene : public Clonable_Gene<Pawn_Structure_Gene>
{
    public:
        Pawn_Structure_Gene() noexcept;
        std::string name() const noexcept override;

    private:
        Gene_Value guarded_by_pawn = {"Guarded By Pawn", 1.0, 0.03};
        Gene_Value guarded_by_piece = {"Guarded By Piece", 1.0, 0.03};

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
        void gene_specific_mutation() noexcept override;
        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;
        void normalize_guard_scores() noexcept;
};

#endif // PAWN_STRUCTURE_GENE_H
