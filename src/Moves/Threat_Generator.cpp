#include "Moves/Threat_Generator.h"

#include <cmath>

#include "Game/Board.h"
#include "Moves/Threat_Iterator.h"

Threat_Generator::Threat_Generator(char target_file_in,
                                   int  target_rank_in,
                                   Color attack_color,
                                   const Board& reference_board) :
    start(target_file_in, target_rank_in, attack_color, reference_board),
    finish(start.make_end_iterator())
{
}

Threat_Iterator Threat_Generator::begin() const
{
    return start;
}

Threat_Iterator Threat_Generator::end() const
{
    return finish;
}
