#ifndef OPENING_MOVE_GENE_H
#define OPENING_MOVE_GENE_H

#include "Gene.h"

#include <map>
#include <string>

#include "Game/Color.h"

class Move;
class Board;

//! \brief A gene that chooses the first move of the game ahead of time.
class Opening_Move_Gene : public Clonable_Gene<Opening_Move_Gene>
{
    public:
        //! \brief Index for locating the gene in the genome
        static constexpr size_t genome_index = 2;

        std::string name() const noexcept override;

        //! \brief Returns a pre-chosen first move for the board position, should one exist, nullptr if not.
        const Move* first_move_choice(const Board& board) const noexcept;

    private:
        const Move* opening_move_choice = nullptr;

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
        void gene_specific_mutation() noexcept override;
        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;
};

#endif // OPENING_MOVE_GENE_H
