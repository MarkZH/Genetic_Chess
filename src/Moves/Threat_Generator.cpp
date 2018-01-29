#include "Moves/Threat_Generator.h"

#include <cmath>

#include "Game/Board.h"
#include "Moves/Threat_Iterator.h"

Threat_Generator::Threat_Generator(char target_file_in,
                                   int  target_rank_in,
                                   Color attack_color,
                                   const Board& reference_board) :
    target_file(target_file_in),
    target_rank(target_rank_in),
    attacking_color(attack_color),
    board(reference_board)
{
}

Threat_Iterator Threat_Generator::begin() const
{
    return Threat_Iterator(target_file, target_rank, attacking_color, board);
}

Threat_Iterator Threat_Generator::end() const
{
    auto t = Threat_Iterator(starting_file, starting_rank, attacking_color, board);
    t.make_end_iterator();
    return t;
}
