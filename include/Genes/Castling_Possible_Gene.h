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
        std::string name() const noexcept override;

    protected:
        std::map<std::string, double> list_properties() const noexcept override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double kingside_preference = 0.0;
        double queenside_preference = 0.0;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
        void gene_specific_mutation() noexcept override;
};

#endif // CASTLING_POSSIBLE_GENE_H
