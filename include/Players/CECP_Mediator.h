#ifndef CECP_MEDIATOR_H
#define CECP_MEDIATOR_H

#include "Outside_Communicator.h"

#include <string>
#include <future>
#include <vector>

class Board;
class Clock;
class Move;
class Player;
class Game_Result;

//! \brief A class to assist in interfacing with a CECP-based external program.
//!
//! Chess Engine Communication Protocol (CECP): https://www.gnu.org/software/xboard/engine-intf.html
class CECP_Mediator : public Outside_Communicator
{
    public:
        //! \brief Set up the connection to the outside interface and send configuration data.
        //!
        //! \param local_player The player on the machine. The name of the player gets sent to the interface.
        explicit CECP_Mediator(const Player& local_player);

        Game_Result setup_turn(Board& board,
                               Clock& clock,
                               std::vector<const Move*>& move_list,
                               const Player& player) override;
        Game_Result handle_move(Board& board,
                                const Move& move,
                                std::vector<const Move*>& move_list) const override;

    private:
        bool in_force_mode = true;
        bool usermove_prefix = true;

        std::string receive_cecp_command(Clock& clock, bool while_listening);
        bool undo_move(std::vector<const Move*>& move_list, std::string& command, Board& board, Clock& clock, const Player& player);
        void send_error(const std::string& command, const std::string& reason) const noexcept;
        std::string listener(Clock& clock) override;
        void report_end_of_game(const Game_Result& ending) const noexcept;
};

#endif // CECP_MEDIATOR_H
