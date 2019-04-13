#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include "Gene.h"

#include <string>
#include <memory>
#include <array>
#include <map>

#include "Game/Color.h"

class Board;
class Move;
class Clock;

//! This gene controls all aspects of time control.
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
        std::map<std::string, double> list_properties() const override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        std::array<double, 4> speculation_constants; // controls over/under-allocation of time
        double mean_game_length; // in moves by one player
        double game_length_uncertainty; // approximately as a fraction of the mean

        double score_board(const Board& board, Color perspective, size_t depth) const override;
        void gene_specific_mutation() override;
};

#endif // LOOK_AHEAD_GENE_H
