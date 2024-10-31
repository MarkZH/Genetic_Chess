#ifndef PGN_H

#include <string>
#include <iostream>
#include <vector>

class Board;
class Move;
class Player;
class Game_Result;
class Clock;

namespace PGN
{
    //! \brief Confirm that all moves in a PGN game record are legal moves.
    //!
    //! \param file_name The name of the file with the PGN game records. All games will be examined.
    //! 
    //! If there is an error in a game record, an exception will be thrown. Otherwise, the number of
    //! games read will be printed to the console.
    void confirm_game_record(const std::string& file_name);

    //! \brief Prints the PGN game record with commentary from Players.
    //!
    //! \param game_record_listing A list of Moves. This must be a legal sequence of moves starting from
    //!        the state of the Board at its creation and resulting in the current state of the Board.
    //! \param white Pointer to Player playing white to provide commentary for moves. Can be nullptr.
    //! \param black Pointer to Player playing black to provide commentary for moves. Can be nullptr.
    //! \param file_name Name of the text file where the game will be printed. If empty, print to stdout.
    //! \param result The result of the last action (move, clock punch, or outside intervention) in the game.
    //! \param game_clock The game clock used during the game.
    //! \param event_name The name of the event where the game will take place. May be empty.
    //! \param location The name of the location of the game. May be empty.
    void print_game_record(const Board& board,
                           const std::vector<const Move*>& game_record_listing,
                           const Player& white,
                           const Player& black,
                           const std::string& file_name,
                           const Game_Result& result,
                           const Clock& game_clock,
                           const std::string& event_name,
                           const std::string& location) noexcept;

    //! \brief Format and print a header line for a PGN game.
    //! 
    //! \tparam Data_Type The type of data be written as the value of the header line.
    //! \param output The stream being written to.
    //! \param heading The name of the header parameter.
    //! \param data The value of the header parameter.
    template<typename Data_Type>
    void print_game_header_line(std::ostream& output, const std::string& heading, const Data_Type& data)
    {
        output << "[" << heading << " \"";
        if constexpr(std::is_same_v<Data_Type, std::string>)
        {
            output << (data.empty() ? "?" : data);
        }
        else
        {
            output << data;
        }
        output << "\"]\n";
    }
}

#endif // !PGN_H

