#include "Moves/Threat_Generator.h"

#include "Moves/Threat_Iterator.h"

class Board;

//! Start the generation of a list of squares that attack a target Square.

//! \param target_file_in The file of the targeted square.
//! \param target_rank_in The rank of the targeted square.
//! \param attack_color The color of pieces attacking the target square.
//! \param reference_board The board on which the action takes place.
Threat_Generator::Threat_Generator(char target_file_in,
                                   int  target_rank_in,
                                   Color attack_color,
                                   const Board& reference_board) :
    start(target_file_in, target_rank_in, attack_color, reference_board),
    finish(start.make_end_iterator())
{
}

//! An iterator to the first square containing an attacking piece.

//! \returns The first iterator referring to an attacking piece.
Threat_Iterator Threat_Generator::begin() const
{
    return start;
}

//! An iterator representing an exhausted list of attacking squares.

//! \returns An iterator that does not refer to an attacking piece.
Threat_Iterator Threat_Generator::end() const
{
    return finish;
}

//! Check if there are no attackers.

//! \returns True if there are no pieces attacking the target square.
bool Threat_Generator::empty() const
{
    return begin() == end();
}
