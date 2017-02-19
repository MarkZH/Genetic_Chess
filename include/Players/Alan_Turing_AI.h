#ifndef ALAN_TURING_AI_H
#define ALAN_TURING_AI_H

#include "Players/Player.h"


class Alan_Turing_AI : public Player
{
    public:
        Alan_Turing_AI();
        ~Alan_Turing_AI();

        std::string name() const override;
        std::string author() const override;
};

#endif // ALAN_TURING_AI_H
