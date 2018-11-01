#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

class Board;
class Move;
class Clock;

// All aspects of time control
class Look_Ahead_Gene : public Gene
{
    public:
        Look_Ahead_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

        // How much time to use in choosing a move
        double time_to_examine(const Board& board, const Clock& clock) const;

        // How much extra time to allocate due to alpha-beta pruning
        double speculation_time_factor(const Board& board) const;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double mean_game_length; // in moves by one player
        double game_length_uncertainty; // approximately as a fraction of the mean

        // how much extra time to allocate by default
        double speculation_constant;

        // how much extra time to allocate when capture moves are possible (but not taken)
        double can_capture_speculation_constant;

        // how much extra time to allocate when the current move captures
        double will_capture_speculation_constant;

        // how much extra time to allocate when last move captured and current move captured
        // (i.e., the move under consideration recaptures)
        double recapture_speculation_constant;


        double score_board(const Board& board, const Board& opposite_board, size_t depth) const override;
        void gene_specific_mutation() override;
};

#endif // LOOK_AHEAD_GENE_H
