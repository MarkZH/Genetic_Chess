#ifndef UCI_MEDIATOR_H
#define UCI_MEDIATOR_H

#include "Outside_Communicator.h"

#include <string>
#include <future>
#include <vector>

class Player;
class Board;
class Clock;
class Move;
class Game_Result;

//! \brief A class that mediates communication with a GUI via the UCI protocol.
class UCI_Mediator : public Outside_Communicator
{
    public:
        //! \brief Setup a connection to the outside world with a UCI interface.
        //!
        //! \param player The chess player on the local machine whose name and author get sent to the GUI.
        explicit UCI_Mediator(const Player& player, bool enable_logging);

        Game_Result setup_turn(Board& board,
                               Clock& clock,
                               std::vector<const Move*>& move_list,
                               const Player& player) override;
        Game_Result handle_move(Board& board,
                                const Move& move,
                                std::vector<const Move*>& move_list) const override;

    private:
        std::string listener(Clock& clock) override;
        std::string receive_uci_command(bool while_listening);
};

#endif // UCI_MEDIATOR_H
