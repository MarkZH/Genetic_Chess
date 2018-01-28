#ifndef THREAT_GENERATOR_H
#define THREAT_GENERATOR_H

#include "Game/Board.h"

class Threat_Iterator;

class Threat_Generator
{
    public:
        Threat_Generator(char file_start,
                         int rank_start,
                         Color attack_color,
                         const Board& refernce_board);

        Threat_Iterator begin() const;
        Threat_Iterator end() const;

    private:
        char starting_file;
        int  starting_rank;
        Color attacking_color;
        const Board& board;
};

#endif // THREAT_GENERATOR_H
