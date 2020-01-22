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

//! A class that mediates communication with a GUI via the UCI protocol.
class UCI_Mediator : public Outside_Communicator
{
    public:
        //! Setup a connection to the outside world with a UCI interface.
        //
        //! \param player The chess player on the local machine whose name and author get sent to the GUI.
        explicit UCI_Mediator(const Player& player);

        void setup_turn(Board& board, Clock& clock, std::vector<const Move*>& move_list) override;
        void listen(Board& board, Clock& clock) override;
        Game_Result handle_move(Board& board, const Move& move) const override;
        bool pondering_allowed() const override;

    private:
        std::future<std::string> last_listening_result;

        std::string listener(Board& board);
        std::string receive_uci_command(Board& board, bool while_listening);
};

#endif // UCI_MEDIATOR_H
