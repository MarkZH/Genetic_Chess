#ifndef RANDOM_AI_H
#define RANDOM_AI_H

#include "AI_Player.h"

class Random_AI : public AI_Player
{
    public:
        const Complete_Move choose_move(const Board& board, const Clock& clock) const override;
        std::string name() const override;
};

#endif // RANDOM_AI_H
