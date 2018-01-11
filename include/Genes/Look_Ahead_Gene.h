#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

class Board;
class Clock;

// All aspects of time control
class Look_Ahead_Gene : public Gene
{
    public:
        Look_Ahead_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

        double time_to_examine(const Board& board, const Clock& clock) const;
        double speculation_time_factor(const Board& board) const;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double mean_game_length; // in moves by one player
        double game_length_uncertainty; // approximately as a fraction of the mean

        double speculation_constant; // how often to look ahead even with little time
        double capturing_speculation_constant; // how often to look ahead even with little time when capture is possible

        double score_board(const Board& board) const override;
        void gene_specific_mutation() override;
};

#endif // LOOK_AHEAD_GENE_H
