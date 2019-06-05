#include "Players/CECP_Mediator.h"

#include <string>
#include <future>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"

#include "Moves/Move.h"

#include "Players/Player.h"

#include "Exceptions/Illegal_Move.h"
#include "Exceptions/Game_Ended.h"

#include "Utility/String.h"

//! Set up the connection to the outside interface and send configuration data.
//
//! \param local_player The player on the machine. The name of the player gets sent to the interface.
CECP_Mediator::CECP_Mediator(const Player& local_player)
{
    std::string expected = "protover 2";
    if(receive_command() == expected)
    {
        log("as expected, setting options");
        send_command("feature "
                     "usermove=1 "
                     "sigint=0 "
                     "sigterm=0 "
                     "reuse=0 "
                     "myname=\"" + local_player.name() + "\" "
                     "name=1 "
                     "ping=1 "
                     "setboard=1 "
                     "done=1");
    }
    else
    {
        log("ERROR: Expected \"" + expected + "\"");
        throw std::runtime_error("Error in communicating with CECP program.");
    }
}

void CECP_Mediator::setup_turn(Board& board, Clock& clock)
{
    log("CECP_Mediator::setup_turn()");

    while(true)
    {
        auto command = receive_cecp_command(board, clock, false);
        if(command == "go")
        {
            if(wait_for_usermove)
            {
                log("Ignoring 'go' command, waiting for usermove");
                continue;
            }

            indent = board.whose_turn() == WHITE ? "\t\t" : "\t\t\t";
            log("telling local AI to move at leisure and accepting move");
            ignore_next_move = false;
            wait_for_usermove = true;
            board.choose_move_at_leisure();
            log("CECP::setup_turn() exit");
            return;
        }
        else if (String::starts_with(command, "setboard "))
        {
            disable_thinking_output = true;
            log("Disabling thinking output");
            board.set_thinking_mode(NO_THINKING);
            wait_for_usermove = false;
            auto fen = String::split(command, " ", 1).back();
            log("Rearranging board to: " + fen);
            board = Board(fen);
        }
        else if(String::starts_with(command, "usermove "))
        {
            auto move = String::split(command).back();
            try
            {
                log("Applying move: " + move);
                auto result = board.submit_move(board.create_move(move));
                indent = board.whose_turn() == WHITE ? "\t\t" : "\t\t\t";
                if(result.game_has_ended())
                {
                    report_end_of_game(result);
                }
                log("reporting last move to local AI and accepting its move");
                ignore_next_move = false;
                wait_for_usermove = true;
                board.choose_move_at_leisure();
                log("CECP::setup_turn() exit");
                return;
            }
            catch(const Illegal_Move& e)
            {
                send_command("Illegal move (" + std::string(e.what()) + ") " + move);
            }
        }
        else if(String::starts_with(command, "time ") || String::starts_with(command, "otim "))
        {
            auto time = std::stod(String::split(command, " ")[1])/100; // time specified in centiseconds
            auto ai_color = wait_for_usermove ? opposite(board.whose_turn()) : board.whose_turn();
            auto clock_color = String::starts_with(command, "time ") ? ai_color : opposite(ai_color);
            clock.set_time(clock_color, time);
            log("setting " + color_text(clock_color) + "'s time to " + std::to_string(time) + " seconds.");
        }
    }
}

void CECP_Mediator::listen(Board& board, Clock& clock)
{
    log("CECP_Mediator::listen()");
    last_listening_command = std::async(std::launch::async, &CECP_Mediator::listener, this, std::ref(board), std::ref(clock));
    log("CECP_Mediator::listen() exits");
}

void CECP_Mediator::handle_move(Board& board, const Move& move)
{
    log("CECP_Mediator::handle_move()");
    if(ignore_next_move)
    {
        log("Ignoring move: " + move.coordinate_move());
        ignore_next_move = false;
    }
    else
    {
        send_command("move " + move.coordinate_move());
        auto result = board.submit_move(move);
        if(result.game_has_ended())
        {
            report_end_of_game(result);
        }
    }
    log("CECP_Mediator::handle_move() exits");
}

bool CECP_Mediator::pondering_allowed() const
{
    return thinking_on_opponent_time;
}

std::string CECP_Mediator::receive_cecp_command(Board& board, Clock& clock, bool while_listening)
{
    log("CECP_Mediator::receive_cecp_command()");
    while(true)
    {
        std::string command;
        if(while_listening)
        {
            command = receive_command();
        }
        else
        {
            command = last_listening_command.valid() ? last_listening_command.get() : receive_command();
        }

        if(String::starts_with(command, "ping "))
        {
            command[1] = 'o'; // change "ping" to "pong"
            send_command(command);
        }
        else if(String::starts_with(command, "result "))
        {
            log("got result " + command);
            auto winner = NONE;
            if(String::contains(command, "1-0"))
            {
                winner = WHITE;
            }
            else if(String::contains(command, "0-1"))
            {
                winner = BLACK;
            }
            auto data = String::split(String::split(command, "{", 1)[1], "}", 1)[0];
            report_end_of_game(Game_Result(winner, data, false));
        }
        else if(command == "force")
        {
            log("Forcing local AI to pick move and ignoring it");
            board.pick_move_now();
            ignore_next_move = true;
        }
        else if(String::starts_with(command, "level "))
        {
            log("got time specs: " + command);
            auto split = String::split(command);

            log("moves to reset clock = " + split[1]);
            auto reset_moves = String::string_to_size_t(split[1]);
            auto time_split = String::split(split[2], ":");
            auto game_time = 0;
            if(time_split.size() == 1)
            {
                log("game time = " + time_split[0] + " minutes");
                game_time = 60*std::stoi(time_split[0]);
            }
            else
            {
                log("game time = " + time_split[0] + " minutes and " + time_split[1] + " seconds");
                game_time = 60*std::stoi(time_split[0]) + std::stoi(time_split[1]);
            }

            log("increment = " + split[3]);
            auto increment = std::stod(split[3]);
            clock = Clock(game_time, reset_moves, increment, WHITE, false);
        }
        else if(String::starts_with(command, "st "))
        {
            log("got time specs: " + command);
            auto split = String::split(command);
            auto time_per_move = std::stoi(split[1]);
            auto reset_moves = 1;
            auto increment = 0;
            auto game_time = time_per_move;
            clock = Clock(game_time, reset_moves, increment, WHITE, false);
        }
        else if(command == "post")
        {
            if(disable_thinking_output)
            {
                board.set_thinking_mode(NO_THINKING);
                log("Disabling thinking output for CECP");
            }
            else
            {
                board.set_thinking_mode(CECP);
                log("turning on thinking output for CECP");
            }
        }
        else if(command == "nopost")
        {
            board.set_thinking_mode(NO_THINKING);
            log("turning off thinking output for CECP");
        }
        else if(command == "easy")
        {
            log("Turning off pondering");
            thinking_on_opponent_time = false;
        }
        else if(command == "hard")
        {
            log("Turning on pondering");
            thinking_on_opponent_time = true;
        }
        else if(command == "new")
        {
            log("Setting board to standard start position and resetting clock");
            board = Board{};
            clock = Clock(clock.initial_time(), clock.moves_to_reset(), clock.increment(), WHITE, false);
        }
        else
        {
            log("CECP_Mediator::receive_cecp_command() exits");
            return command;
        }
    }
}

std::string CECP_Mediator::listener(Board& board, Clock& clock)
{
    log("CECP_Mediator::listener()");
    while(true)
    {
        auto command = receive_cecp_command(board, clock, true);
        if(command == "?")
        {
            log("Forcing local AI to pick move and accepting it");
            board.pick_move_now();
            ignore_next_move = false;
        }
        else
        {
            log("CECP_Mediator::listener() returns with: " + command);
            return command;
        }
    }
}

void CECP_Mediator::report_end_of_game(const Game_Result& result) const
{
    send_command(result.game_ending_annotation() + " {" + result.ending_reason() + "}");
    throw Game_Ended();
}
