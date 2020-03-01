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
        //! \returns A numerical value of the Piece.
        double piece_value(Piece piece) const noexcept;

        //! \brief Returns the value of all pieces.
        const std::array<double, 6>& piece_values() const noexcept;

    protected:
        std::map<std::string, double> list_properties() const noexcept override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        std::array<double, 6> piece_strength;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
        double piece_value(Piece_Type type) const noexcept;
        double& piece_value(Piece_Type type) noexcept;
        void gene_specific_mutation() noexcept override;
        void renormalize_values() noexcept;
};

#endif // PIECE_STRENGTH_GENE_H
