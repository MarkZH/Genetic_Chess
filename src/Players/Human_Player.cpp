#include "Players/Human_Player.h"

#include <string>
#include <iostream>
#include <cmath>

#include "Game/Board.h"
#include "Game/Clock.h"

#include "Utility/String.h"

class Move;

#include "Exceptions/Illegal_Move.h"
#include "Exceptions/Promotion_Piece_Needed.h"

//! The Human_Player constructor asks for the human's name.
//
//! The name is optional and can be left blank.
Human_Player::Human_Player()
{
    std::cout << "Enter name: ";
    std::getline(std::cin, player_name);
}

//! The interface for the human player is a text console.
//
//! The player can type moves in PGN or coordinate notation.
//! The text-based UI includes the time left. The player can enter a blank or illegal move
//! to update the time. If an illegal or invalid move is entered, the user is asked again
//! to type a move.
//! \param board The current state of the game.
//! \param clock The amount of time left in the game.
const Move& Human_Player::choose_move(const Board& board, const Clock& clock) const
{
    std::string why_illegal;
    std::string move;

    while(true)
    {
        board.ascii_draw(board.whose_turn());
        if( ! why_illegal.empty())
        {
            std::cout << "Illegal move: " << move << " (" << why_illegal << ")" << std::endl;
        }

        std::cout << color_text(board.whose_turn());
        if( ! name().empty())
        {
            std::cout << " (" << name() << ")";
        }
        std::cout << " to move  |  ";
        if( ! board.game_record().empty())
        {
            std::cout << "Last move: "
                      << int(std::ceil(board.game_record().size()/2.0)) << ". "
                      << (board.whose_turn() == WHITE ? "... " : "")
                      << board.last_move_record() << "  |  ";
        }
        std::cout << "Time: " << clock.time_left(board.whose_turn()) << std::endl;

        std::cout << "Enter move: ";
        std::getline(std::cin, move);

        try
        {
            try
            {
                return board.create_move(move);
            }
            catch(const Promotion_Piece_Needed&)
            {
                const std::string choices = "BNRQ";
                std::string promote = "--";
                while(promote.size() != 1 || ! String::contains(String::lowercase(choices), String::lowercase(promote)))
                {
                    std::cout << "What should the pawn be promoted to?\n";
                    std::cout << "Choice: [" << choices << "]: ";
                    std::getline(std::cin, promote);
                    promote = String::trim_outer_whitespace(promote);
                }
                return board.create_move(move + promote);
            }
        }
        catch(const Illegal_Move& e)
        {
            why_illegal = e.what();
        }
    }
}

//! \returns The player's name, if entered.
std::string Human_Player::name() const
{
    return player_name;
}

std::string Human_Player::author() const
{
    return "Mark Harrison";
}
