#ifndef UCI_MEDIATOR_H
#define UCI_MEDIATOR_H

#include <string>

#include "Outside_Player.h"
#include "Game/Color.h"

class Board;
class Clock;
class Move;
class Game_Result;

class UCI_Mediator : public Outside_Player
{
    public:
        explicit UCI_Mediator(const Player& local_player);

        const Move& choose_move(const Board& board, const Clock& clock) const override;
        Color get_ai_color() const override;
        std::string name() const override;
        void process_game_ending(const Game_Result& gee, const Board& board) const override;

    protected:
        std::string receive_move(const Clock& clock) const override;
        void get_clock_specs() override;

    private:
        std::string receive_uci_command() const;
};

#endif // UCI_MEDIATOR_H
