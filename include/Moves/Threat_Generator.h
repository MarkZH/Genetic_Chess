#ifndef THREAT_GENERATOR_H
#define THREAT_GENERATOR_H

#include "Game/Color.h"

class Threat_Iterator;
class Board;

// Generates a list of all moves by attack_color that
// attack Square{file_start, rank_start}.
class Threat_Generator
{
    public:
        Threat_Generator(char target_file_in,
                         int  target_rank_in,
                         Color attack_color,
                         const Board& refernce_board);

        Threat_Iterator begin() const;
        Threat_Iterator end() const;

    private:
        char target_file;
        int  target_rank;
        Color attacking_color;
        const Board& board;
};

#endif // THREAT_GENERATOR_H
