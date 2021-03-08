#ifndef DRAW_VALUE_GENE_H
#define DRAW_VALUE_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"

class Board;


//! \brief Determines the value of a drawn position.
//!
//! The idea here is that, if it is possible to force a draw, the
//! next best move needs to be lower than the value returned by
//! Draw_Value_Gene::draw_value() in order to decide to force the
//! draw. If every other move leaves the player down a pawn, that's
//! not a reason to force a draw.
class Draw_Value_Gene : public Clonable_Gene<Draw_Value_Gene>
{
    public:
        //! \brief Index for locating the gene in the genome
        static const size_t genome_index = 2;

        std::string name() const noexcept override;

        //! Return the genetically determined draw value in centipawns.
        double draw_value() const noexcept;

    private:
        double value_of_draw_in_centipawns = 0.0;

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
        void gene_specific_mutation() noexcept override;

        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
};

#endif // DRAW_VALUE_GENE_H
