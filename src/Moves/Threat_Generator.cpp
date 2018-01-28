#include "Moves/Threat_Generator.h"

#include <cmath>

#include "Game/Board.h"
#include "Moves/Threat_Iterator.h"

Threat_Generator::Threat_Generator(char file_start,
                                   int rank_start,
                                   Color attack_color,
                                   const Board& reference_board) :
    starting_file(file_start),
    starting_rank(rank_start),
    attacking_color(attack_color),
    board(reference_board)
{
}

Threat_Iterator Threat_Generator::begin() const
{
    return Threat_Iterator(starting_file, starting_rank, attacking_color, board);
}

Threat_Iterator Threat_Generator::end() const
{
    auto t = Threat_Iterator(starting_file, starting_rank, attacking_color, board);
    t.make_end_iterator();
    return t;
}
