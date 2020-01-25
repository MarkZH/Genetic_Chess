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

CECP_Mediator::CECP_Mediator(const Player& local_player)
{
    std::string expected = "protover 2";
    if(receive_command() == expected)
    {
        send_command("feature "
                     "usermove=1 "
                     "sigint=0 "
                     "reuse=0 "
                     "myname=\"" + local_player.name() + "\" "
                     "name=1 "
                     "ping=1 "
                     "setboard=1 "
                     "colors=0 "
                     "done=1");
    }
    else
    {
        log("ERROR: Expected \"" + expected + "\"");
        throw std::runtime_error("Error in communicating with CECP program.");
    }
}

void CECP_Mediator::setup_turn(Board& board, Clock& clock, std::vector<const Move*>& move_list)
{
    auto own_time_left = clock.time_left(opposite(board.whose_turn()));
    auto opponent_time_left = clock.time_left(board.whose_turn());

    while(true)
    {
        auto command = receive_cecp_command(board, clock, false);
        if(command == "go")
        {
            log("telling local AI to move at leisure and accepting move");
            in_force_mode = false;
            board.choose_move_at_leisure();
            break;
        }
        else if (String::starts_with(command, "setboard "))
        {
            auto fen = String::split(command, " ", 1).back();
            
            // Handle GUIs that send the next board position
            // instead of a move.
            auto new_move_list = board.derive_moves(fen);
            if(new_move_list.empty())
            {
                try
                {
                    log("Rearranging board to: " + fen);
                    board = Board(fen);
                    move_list.clear();
                }
                catch(const std::invalid_argument&)
                {
                    send_error(command, "Bad FEN");
                }                
            }
            else
            {
                for(auto move : new_move_list)
                {
                    log("Derived move: " + move->coordinate_move());
                    board.submit_move(*move);
                    move_list.push_back(board.last_move());
                }
            }
        }
        else if(String::starts_with(command, "usermove "))
        {
            auto move = String::split(command).back();
            try
            {
                log("Applying move: " + move);
                auto result = board.submit_move(move);
                move_list.push_back(board.last_move());
                if(result.game_has_ended())
                {
                    report_end_of_game(result);
                }

                if( ! in_force_mode)
                {
                    log("Local AI now chooses a move");
                    board.choose_move_at_leisure();
                    break;
                }
            }
            catch(const Illegal_Move& e)
            {
                send_command("Illegal move (" + std::string(e.what()) + ") " + move);
            }
        }
        else if(String::starts_with(command, "time "))
        {
            // time specified in centiseconds
            own_time_left = std::stod(String::split(command, " ")[1])/100;
            log("Will set own time to " + std::to_string(own_time_left));
        }
        else if(String::starts_with(command, "otim "))
        {
            // time specified in centiseconds
            opponent_time_left = std::stod(String::split(command, " ")[1])/100;
            log("Will set opponent's time to " + std::to_string(opponent_time_left));
        }
        else if(command == "undo")
        {
            undo_move(move_list, command, board, clock);
        }
        else if(command == "remove")
        {
            if(undo_move(move_list, command, board, clock))
            {
                undo_move(move_list, command, board, clock);
            }
        }
    }

    log("Setting own time (" + color_text(board.whose_turn()) + ") to " + std::to_string(own_time_left));
    clock.set_time(board.whose_turn(), own_time_left);
    log("Setting opponent's time (" + color_text(opposite(board.whose_turn())) + ") to " + std::to_string(opponent_time_left));
    clock.set_time(opposite(board.whose_turn()), opponent_time_left);

    if( ! clock.is_running())
    {
        clock.start();
    }

    if(clock.running_for() != board.whose_turn())
    {
        clock.punch(board);
    }
}

bool CECP_Mediator::undo_move(std::vector<const Move*>& move_list, std::string& command, Board& board, Clock& clock)
{
    if(move_list.empty())
    {
        send_error(command, "no moves to undo");
        return false;
    }
    else
    {
        log("Undoing move: " + move_list.back()->coordinate_move());
        move_list.pop_back();
        auto new_board = Board(board.original_fen());
        for(auto move : move_list)
        {
            new_board.submit_move(*move);
        }
        board = new_board;
        clock.unpunch();
        return true;
    }
}

void CECP_Mediator::listen(Board& board, Clock& clock)
{
    last_listening_command = std::async(std::launch::async, &CECP_Mediator::listener, this, std::ref(board), std::ref(clock));
}

Game_Result CECP_Mediator::handle_move(Board& board, const Move& move, std::vector<const Move*>& move_list) const
{
    if(in_force_mode)
    {
        log("Ignoring move: " + move.coordinate_move());
        return {};
    }
    else
    {
        send_command("move " + move.coordinate_move());
        move_list.push_back(&move);
        auto result = board.submit_move(move);
        if(result.game_has_ended())
        {
            report_end_of_game(result);
        }
        return result;
    }
}

bool CECP_Mediator::pondering_allowed() const
{
    return thinking_on_opponent_time;
}

std::string CECP_Mediator::receive_cecp_command(Board& board, Clock& clock, bool while_listening)
{
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
            auto result = String::split(command).at(1);
            auto reason = String::extract_delimited_text(command, "{", "}");
            report_end_of_game(result, reason);
        }
        else if(command == "force")
        {
            log("Entering force mode");
            board.pick_move_now();
            clock.stop();
            in_force_mode = true;
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
            clock = Clock(game_time, reset_moves, increment, WHITE);
        }
        else if(String::starts_with(command, "st "))
        {
            log("got time specs: " + command);
            auto split = String::split(command);
            auto time_per_move = std::stoi(split[1]);
            auto reset_moves = 1;
            auto increment = 0;
            auto game_time = time_per_move;
            clock = Clock(game_time, reset_moves, increment, WHITE);
        }
        else if(command == "post")
        {
            if(board.thinking_mode() == Thinking_Output_Type::CECP)
            {
                board.set_thinking_mode(Thinking_Output_Type::NO_THINKING);
                log("Disabling thinking output for CECP");
            }
            else
            {
                board.set_thinking_mode(Thinking_Output_Type::CECP);
                log("turning on thinking output for CECP");
            }
        }
        else if(command == "nopost")
        {
            board.set_thinking_mode(Thinking_Output_Type::NO_THINKING);
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
            clock = Clock(clock.initial_time(), clock.moves_per_time_period(), clock.increment(WHITE), WHITE);
            in_force_mode = false;
        }
        else if(String::starts_with(command, "name "))
        {
            log("Getting other player's name");
            set_other_player_name(String::split(command, " ", 1).back());
        }
        else
        {
            return command;
        }
    }
}

void CECP_Mediator::send_error(const std::string& command, const std::string& reason) const noexcept
{
    send_command("Error (" + reason + "): " + command);
}

std::string CECP_Mediator::listener(Board& board, Clock& clock)
{
    while(true)
    {
        auto command = receive_cecp_command(board, clock, true);
        if(command == "?")
        {
            log("Forcing local AI to pick move and accepting it");
            board.pick_move_now();
        }
        else
        {
            return command;
        }
    }
}

void CECP_Mediator::report_end_of_game(const std::string& result, const std::string& reason) const
{
    send_command(result + " {" + reason + "}");
    throw Game_Ended(reason);
}

void CECP_Mediator::report_end_of_game(const Game_Result& result) const
{
    report_end_of_game(result.game_ending_annotation(), result.ending_reason());
}
