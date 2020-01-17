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
        //! Set up the connection to the outside interface and send configuration data.
        //
        //! \param local_player The player on the machine. The name of the player gets sent to the interface.
        explicit CECP_Mediator(const Player& local_player);

        void setup_turn(Board& board, Clock& clock) override;
        void listen(Board& board, Clock& clock) override;
        Game_Result handle_move(Board& board, const Move& move) const override;
        bool pondering_allowed() const override;

    private:
        std::future<std::string> last_listening_command;
        bool thinking_on_opponent_time = false;
        bool in_force_mode = true;

        std::string receive_cecp_command(Board& board, Clock& clock, bool while_listening);
        std::string listener(Board& board, Clock& clock);
        void report_end_of_game(const std::string& result, const std::string& reason) const;
        void report_end_of_game(const Game_Result& ending) const;
};

#endif // CECP_MEDIATOR_H
