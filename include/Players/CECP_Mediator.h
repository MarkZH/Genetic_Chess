#ifndef CECP_MEDIATOR_H
#define CECP_MEDIATOR_H

#include "Outside_Player.h"
#include "Game/Color.h"

#include <string>

class Board;
class Clock;

class CECP_Mediator : public Outside_Player
{
    public:
        CECP_Mediator(const std::string& name);

        const Complete_Move choose_move(const Board& b, const Clock& clock) const override;
        std::string receive_move() const override;

        Color get_ai_color() const override;
        void process_game_ending(const Game_Ending_Exception& gee, const Board& board) const override;

        std::string name() const override;

    private:
        mutable std::string first_move;
        mutable std::string received_name;

        void get_clock_specs() override;
        std::string receive_cecp_command() const;
};

#endif // CECP_MEDIATOR_H
