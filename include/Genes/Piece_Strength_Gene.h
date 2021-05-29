#ifndef PIECE_STRENGTH_GENE_H
#define PIECE_STRENGTH_GENE_H

#include "Gene.h"

#include <string>
#include <array>
#include <map>

#include "Game/Color.h"
#include "Game/Piece.h"

class Board;

//! \brief Provides a database of the value of different piece types to be referenced by other Genes.
class Piece_Strength_Gene : public Clonable_Gene<Piece_Strength_Gene>
{
    public:
        //! \brief Index for locating the gene in the genome
        static const size_t genome_index = 0;

        //! \brief Initialize the Piece values to all be the same.
        Piece_Strength_Gene() noexcept;

        std::string name() const noexcept override;

        //! \brief This method is queried by other Genes to determine the value of various Pieces.
        //!
        //! \param piece A piece--which may be from an empty square--to valuate.
        //! \returns A numerical value of the Piece.
        double piece_value(Piece piece) const noexcept;

        //! \brief Returns the value of all pieces.
        const std::array<double, 6>& piece_values() const noexcept;

        //! \brief Estimate the fraction of the game completed by the value of all pieces removed from the board.
        //!
        //! \param board The board to consider.
        //! \returns The total value of the pieces removed from the board from the side that has lost the most
        //!          pieces in terms of value. Only the side that has lost the most pieces is considered since,
        //!          if one side has lost a lot more pieces than the other, then the game is nearly over.
        double game_progress(const Board& board) const noexcept;

    private:
        std::array<double, 6> piece_strength;

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
        double piece_value(Piece_Type type) const noexcept;
        double& piece_value(Piece_Type type) noexcept;
        void gene_specific_mutation() noexcept override;
        void renormalize_values() noexcept;
        double normalization() const noexcept;
        double file_normalization() const noexcept;
};

#endif // PIECE_STRENGTH_GENE_H
