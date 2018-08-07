#include "Players/Human_Player.h"

#include <string>
#include <iostream>
#include <cmath>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Moves/Move.h"

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
            why_illegal.clear();
        }
        std::cout << color_text(board.whose_turn());
        if( ! name().empty())
        {
            std::cout << " (" << name() << ")";
        }
        std::cout << " to move  |  ";
        if( ! board.get_game_record().empty())
        {
            std::cout << "Last move: "
                      << int(std::ceil(board.get_game_record().size()/2.0)) << ". "
                      << (board.whose_turn() == WHITE ? "... " : "")
                      << board.get_last_move_record() << "  |  ";
        }
        std::cout << "Time: " << clock.time_left(board.whose_turn()) << std::endl;
        std::cout << "Enter move: ";
        std::getline(std::cin, move);
        try
        {
            board.ascii_draw(opposite(board.whose_turn()));
            return board.get_move(move);
        }
        catch(const Illegal_Move& e)
        {
            why_illegal = e.what();
        }
        catch(const Promotion_Piece_Needed&)
        {
            std::cout << "What should the pawn be promoted to?\n";
            std::cout << "Choice: [B N R Q]: ";
            char promote;
            std::cin >> promote;
            try
            {
                board.ascii_draw(opposite(board.whose_turn()));
                return board.get_move(move, promote);
            }
            catch(const Illegal_Move& e)
            {
                why_illegal = e.what();
            }
        }
    }
}

std::string Human_Player::name() const
{
    return player_name;
}
