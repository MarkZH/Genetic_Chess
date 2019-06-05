#ifndef CECP_MEDIATOR_H
#define CECP_MEDIATOR_H

#include "Outside_Communicator.h"

#include <string>
#include <future>

class Board;
class Clock;
class Move;
class Player;
class Game_Result;

//! A class to assist in interfacing with a CECP-based external program.
//
//! Chess Engine Communication Protocol (CECP): https://www.gnu.org/software/xboard/engine-intf.html
class CECP_Mediator : public Outside_Communicator
{
    public:
        explicit CECP_Mediator(const Player& local_player);

        void setup_turn(Board& board, Clock& clock) override;
        void listen(Board& board, Clock& clock) override;
        void handle_move(Board& board, const Move& move) override;
        bool pondering_allowed() const override;

    private:
        bool ignore_next_move = false;
        std::future<std::string> last_listening_command;
        bool disable_thinking_output = false;
        bool thinking_on_opponent_time = false;
        bool wait_for_usermove = false;

        std::string receive_cecp_command(Board& board, Clock& clock, bool while_listening);
        std::string listener(Board& board, Clock& clock);
        void report_end_of_game(const Game_Result& result) const;
};

#endif // CECP_MEDIATOR_H
