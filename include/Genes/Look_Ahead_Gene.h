#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"
#include "Game/Clock.h"

class Board;

//! This gene controls all aspects of time control.
class Look_Ahead_Gene : public Clonable_Gene<Look_Ahead_Gene>
{
    public:
        Look_Ahead_Gene() noexcept;

        std::string name() const noexcept override;

        //! \brief How much time to search for the next move.
        //!
        //! \param board The current board position.
        //! \param clock The game clock.
        //! \returns The total time in seconds to spend on searching for a move.
        Clock::seconds time_to_examine(const Board& board, const Clock& clock) const noexcept;

        //! \brief When searching for a move, determine how much to overcommit on time.
        //!
        //! With alpha-beta pruning, only a portion of a branch will be searched, so time
        //! will be saved that can be spent on other branches. So, this factor controls
        //! how much extra time to allocate knowing that not all of it will be used.
        //! \returns A factor that gets multiplied by the allocated time to overallocate.
        double speculation_time_factor() const noexcept;

    private:
        double speculation_constant = 1.0; // controls over/under-allocation of time
        double mean_game_length = 50.0; // in moves by one player
        double game_length_uncertainty = 0.5; // approximately as a fraction of the mean

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
        void gene_specific_mutation() noexcept override;

        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
};

#endif // LOOK_AHEAD_GENE_H
