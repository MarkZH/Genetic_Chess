#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include "Gene.h"

#include <string>
#include <memory>
#include <map>

#include "Game/Color.h"

class Board;
class Clock;

//! This gene controls all aspects of time control.
class Look_Ahead_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

        //! How much time to search for the next move.
        //
        //! \param board The current board position.
        //! \param clock The game clock.
        //! \returns The total time in seconds to spend on searching for a move.
        double time_to_examine(const Board& board, const Clock& clock) const;

        //! When searching for a move, determine how much to overcommit on time.
        //
        //! With alpha-beta pruning, only a portion of a branch will be searched, so time
        //! will be saved that can be spent on other branches. So, this factor controls
        //! how much extra time to allocate knowing that not all of it will be used.
        //! \returns A factor that gets multiplied by the allocated time to overallocate.
        double speculation_time_factor() const;

    protected:
        std::map<std::string, double> list_properties() const override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double speculation_constant = 1.0; // controls over/under-allocation of time
        double mean_game_length = 50.0; // in moves by one player
        double game_length_uncertainty = 0.5; // approximately as a fraction of the mean

        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const override;
        void gene_specific_mutation() override;
};

#endif // LOOK_AHEAD_GENE_H
