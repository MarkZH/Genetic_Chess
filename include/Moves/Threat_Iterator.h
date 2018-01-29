#ifndef THREAT_ITERATOR_H
#define THREAT_ITERATOR_H

#include "Game/Color.h"

class Board;
class Move;

// The iterator created by Threat_Generator that creates
// all moves by attack_color that attack Square{file_start, rank_start}.
class Threat_Iterator
{
    public:
        Threat_Iterator(char target_file_in,
                        int  target_rank_in,
                        Color attack_color,
                        const Board& reference_board);

        Threat_Iterator& operator++();
        Move operator*() const;
        bool operator!=(const Threat_Iterator& other) const;

        void make_end_iterator();

    private:
        char target_file;
        int  target_rank;

        int file_step;
        int rank_step;
        int step_size;

        Color attacking_color;
        const Board& board;

        bool is_a_threat() const;
        void next_threat();
        char attacking_file() const;
        int  attacking_rank() const;
        bool way_blocked() const;
};

#endif // THREAT_ITERATOR_H
