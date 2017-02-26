#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include "Genes/Gene.h"

class Board;
class Clock;

class Look_Ahead_Gene : public Gene
{
    public:
        Look_Ahead_Gene();
        ~Look_Ahead_Gene() override;

        Look_Ahead_Gene* duplicate() const override;

        std::string name() const override;

        double time_to_examine(const Board& board, const Clock& clock) const;
        double minimum_time_to_recurse(const Board& board) const;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double mean_game_length; // in moves by one player
        double positions_per_second; // number of positions examined per second

        double score_board(const Board& board, Color perspective) const override;
        void gene_specific_mutation() override;
};

#endif // LOOK_AHEAD_GENE_H
