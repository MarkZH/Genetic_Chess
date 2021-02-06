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
        //! \brief Initialize the Piece values to zero.
        Piece_Strength_Gene() noexcept;

        std::string name() const noexcept override;

        //! \brief This method is queried by other Genes to determine the value of various Pieces.
        //!
        //! \param piece A pointer to a Piece. A nullptr represents no piece (as from an empty square).
        //! \param game_progress An estimate of the fraction of the game that has elapsed.
        //! \returns A numerical value of the Piece.
        double piece_value(Piece piece, double game_progress) const noexcept;

        //! \brief Returns the value of all pieces.
        //!
        //! \param game_progress An estimate of the fractional amount of game that has occurred.
        std::array<double, 6> piece_values(double game_progress) const noexcept;

    private:
        std::array<double, 6> opening_piece_strength;
        std::array<double, 6> endgame_piece_strength;

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
        static double piece_value(const std::array<double, 6>& piece_strength, Piece_Type type) noexcept;
        static double& piece_value(std::array<double, 6>& piece_strength, Piece_Type type) noexcept;
        void gene_specific_mutation() noexcept override;
        void renormalize_values() noexcept;
        static double normalization(const std::array<double, 6>& piece_strengths) noexcept;
};

#endif // PIECE_STRENGTH_GENE_H
