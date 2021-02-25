#ifndef TOTAL_FORCE_GENE_H
#define TOTAL_FORCE_GENE_H

#include "Gene.h"

#include <string>
#include <vector>

#include "Game/Color.h"
#include "Game/Piece.h"

class Board;
class Piece_Strength_Gene;

//! \brief Scores a board based on the weighted sum of all a player's pieces.
//!
//! The weights are provided by the Piece_Strength_Gene.
class Total_Force_Gene : public Clonable_Gene<Total_Force_Gene>
{
    public:
        //! \brief The Total_Force_Gene constructor requires a Piece_Strength_Gene to reference in score_board().
        //!
        //! \param piece_strength_source The source of piece values to weight the importance of various pieces.
        explicit Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source) noexcept;

        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept override;

        std::string name() const noexcept override;

    private:
        const Piece_Strength_Gene* piece_strength_source;
        double gate_factor = 1.0;

        void gene_specific_mutation(const std::vector<Piece_Type>&) noexcept override;
        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
};

#endif // TOTAL_FORCE_GENE_H
