#include "Players/Human_Player.h"

#include <string>
#include <iostream>
#include <cmath>
#include <stdexcept>

#include "Game/Board.h"
#include "Game/Clock.h"

#include "Utility/String.h"

class Move;

#include "Exceptions/Illegal_Move.h"
#include "Exceptions/Promotion_Piece_Needed.h"

Human_Player::Human_Player()
{
    std::cout << "Enter name: ";
    std::getline(std::cin, player_name);
}

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

        if( ! std::getline(std::cin, move))
        {
            std::cout << "\n\n";
            throw std::runtime_error(String::trim_outer_whitespace(name() + " disconnected"));
        }

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
        catch(const Illegal_Move& e)
        {
            why_illegal = e.what();
        }
    }
}

std::string Human_Player::name() const
{
    return player_name;
}

std::string Human_Player::author() const
{
    return "Mark Harrison";
}
