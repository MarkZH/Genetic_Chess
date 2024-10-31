#ifndef XBOARD_MEDIATOR_H
#define XBOARD_MEDIATOR_H

#include "Outside_Communicator.h"

#include <string>
#include <future>
#include <vector>

class Board;
class Clock;
class Move;
class Move_Decision;
class Player;
class Game_Result;

//! \brief A class to assist in interfacing with a Xboard-based external program.
//!
//! Chess Engine Communication Protocol (a.k.a., Xboard): https://www.gnu.org/software/xboard/engine-intf.html
class Xboard_Mediator : public Outside_Communicator
{
    public:
        //! \brief Set up the connection to the outside interface and send configuration data.
        //!
        //! \param local_player The player on the machine. The name of the player gets sent to the interface.
        explicit Xboard_Mediator(const Player& local_player, bool enable_logging);

        Game_Result setup_turn(Board& board,
                               Clock& clock,
                               std::vector<const Move*>& move_list,
                               const Player& player) override;
        Game_Result handle_decision(Board& board,
                                    const Move_Decision& decision,
                                    std::vector<const Move*>& move_list) const override;

    private:
        bool in_force_mode = true;
        bool usermove_prefix = true;

        std::string receive_xboard_command(Clock& clock, bool while_listening);
        bool undo_move(std::vector<const Move*>& move_list, const std::string& command, Board& board, Clock& clock, const Player& player);
        void send_error(const std::string& command, const std::string& reason) const noexcept;
        std::string listener(Clock& clock) override;
        void report_end_of_game(const Game_Result& ending) const noexcept;
};

#endif // XBOARD_MEDIATOR_H
