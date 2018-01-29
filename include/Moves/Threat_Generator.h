#ifndef THREAT_GENERATOR_H
#define THREAT_GENERATOR_H

#include "Game/Color.h"
#include "Moves/Threat_Iterator.h"

class Board;

// Generates a list of all squares that that
// attack Square{file_start, rank_start}.
class Threat_Generator
{
    public:
        Threat_Generator(char target_file_in,
                         int  target_rank_in,
                         Color attack_color,
                         const Board& reference_board);

        Threat_Iterator begin() const;
        Threat_Iterator end() const;
        bool empty() const;

    private:
        Threat_Iterator start;
        Threat_Iterator finish;
};

#endif // THREAT_GENERATOR_H
