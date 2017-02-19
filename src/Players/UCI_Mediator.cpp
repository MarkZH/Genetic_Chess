#include "Players/UCI_Mediator.h"

#include "Players/UCI_Mediator.h"

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Game/Clock.h"

#include "Exceptions/Illegal_Move_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"

#include "Utility.h"

UCI_Mediator::UCI_Mediator(const Player& local_player)
{
    send_command("id name " + local_player.name());
    send_command("id author " + local_player.author());
    send_command("uciok");
    auto response = receive_uci_command();
    if(response == "isready")
    {
        send_command("readok");
    }
    response = receive_uci_command();
    if(response == "ucinewgame")
    {
        log("starting new game");
    }
    else
    {
        log("unexpected response");
    }
}

UCI_Mediator::~UCI_Mediator()
{
}

const Complete_Move UCI_Mediator::choose_move(const Board& board, const Clock& clock) const
{
    while(true)
    {
        auto command = receive_uci_command();
    }
}

Color UCI_Mediator::get_ai_color() const
{
    while(true)
    {
        auto cmd = receive_uci_command();
        if(String::starts_with(cmd, "position"))
        {
            log("stopping here for now");
            throw std::runtime_error("Debugging");
        }
    }
}

std::string UCI_Mediator::receive_move(const Clock& clock) const
{
    while(true)
    {
        auto move = receive_uci_command();
    }
}

std::string UCI_Mediator::name() const
{
    return "UCI Interface Player";
}

void UCI_Mediator::process_game_ending(const Game_Ending_Exception&, const Board& board) const
{
    send_command("bestmove " + board.last_move_coordinates());
}

void UCI_Mediator::get_clock_specs()
{
}

std::string UCI_Mediator::receive_uci_command() const
{
    return receive_command();
}
