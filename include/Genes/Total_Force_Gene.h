#ifndef TOTAL_FORCE_GENE_H
#define TOTAL_FORCE_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

class Board;
class Piece_Strength_Gene;

//! \brief Scores a board based on the weighted sum of all a player's pieces.
//!
//! The weights are provided by the Piece_Strength_Gene.
class Total_Force_Gene : public Clonable_Gene<Total_Force_Gene>
{
    public:
        //! \brief Index for locating the gene in the genome
        static const size_t genome_index = 2;

        //! \brief The Total_Force_Gene constructor requires a Piece_Strength_Gene to reference in score_board().
        //!
        //! \param piece_strength_source The source of piece values to weight the importance of various pieces.
        explicit Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source) noexcept;

        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept override;

        std::string name() const noexcept override;

        //! \brief Estimate the fraction of the game completed by the value of all pieces removed from the board.
        //!
        //! \param board The board to consider.
        //! \returns The total value of the pieces removed from the board from the side that has lost the most
        //!          pieces in terms of value. Only the side that has lost the most pieces is considered since,
        //!          if one side has lost a lot more pieces than the other, then the game is nearly over.
        double game_progress(const Board& board) const noexcept;

    private:
        const Piece_Strength_Gene* piece_strength_source;

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
};

#endif // TOTAL_FORCE_GENE_H
