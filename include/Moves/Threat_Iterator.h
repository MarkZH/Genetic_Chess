#ifndef THREAT_ITERATOR_H
#define THREAT_ITERATOR_H

#include "Game/Board.h"
#include "Game/Color.h"

class Move;

class Threat_Iterator
{
    public:
        Threat_Iterator(char file_start,
                        int  rank_start,
                        Color attack_color,
                        const Board& reference_board);

        Threat_Iterator& operator++();
        Move operator*() const;
        bool operator!=(const Threat_Iterator& other) const;

        void make_end_iterator();

    private:
        char starting_file;
        int  starting_rank;

        int file_step;
        int rank_step;
        int step_size;

        Color attacking_color;
        const Board& board;

        bool is_a_threat() const;
        void next_threat();
        char ending_file() const;
        int  ending_rank() const;
};

#endif // THREAT_ITERATOR_H
