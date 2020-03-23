#include "Players/CECP_Mediator.h"

#include <string>
#include <future>
#include <chrono>
using namespace std::chrono_literals;

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"

#include "Moves/Move.h"

#include "Players/Player.h"

#include "Utility/Exceptions.h"
#include "Utility/String.h"

CECP_Mediator::CECP_Mediator(const Player& local_player)
{
    std::string expected = "protover 2";
    if(receive_command() == expected)
    {
        send_command("feature "
                     "usermove=1 "
                     "sigint=0 "
                     "sigterm=0 "
                     "reuse=1 "
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

Game_Result CECP_Mediator::setup_turn(Board& board, Clock& clock, std::vector<const Move*>& move_list, const Player& player)
{
    using centiseconds = std::chrono::duration<int, std::centi>;

    auto own_time_left = clock.time_left(opposite(board.whose_turn()));
    auto opponent_time_left = clock.time_left(board.whose_turn());
    auto need_to_set_time = false;
    Game_Result setup_result;

    while(true)
    {
        std::string command;
        try
        {
            command = receive_cecp_command(board, clock, false);
        }
        catch(const Game_Ended& game_ending_error)
        {
            return Game_Result(Winner_Color::NONE, game_ending_error.what(), true);
        }

        board.pick_move_now(); // Stop pondering

        if(command == "go")
        {
            log("telling local AI to move at leisure and accepting move");
            in_force_mode = false;
            break;
        }
        else if(command == "new")
        {
            log("Setting board to standard start position and resetting clock");
            board = Board{};
            clock = Clock(clock.initial_time(), clock.moves_per_time_period(), clock.increment(Piece_Color::WHITE), clock.reset_mode(), Piece_Color::WHITE);
            move_list.clear();
            need_to_set_time = false;;
            setup_result = {};
            player.reset();
            in_force_mode = false;
        }
        else if(String::starts_with(command, "name "))
        {
            auto name = String::split(command, " ", 1).back();
            log("Getting other player's name: " + name);
            player.set_opponent_name(name);
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
                    setup_result = {};
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
                    log("Derived move: " + move->coordinates());
                    setup_result = board.submit_move(*move);
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
                setup_result = board.submit_move(move);
                move_list.push_back(board.last_move());
                if(setup_result.game_has_ended())
                {
                    report_end_of_game(setup_result);
                }

                if( ! in_force_mode)
                {
                    log("Local AI now chooses a move");
                    break;
                }
            }
            catch(const Illegal_Move& e)
            {
                send_command("Illegal move (" + std::string(e.what()) + ") " + move);
            }
        }
        else if(String::starts_with(command, "level "))
        {
            log("got time specs: " + command);
            auto split = String::split(command);

            log("moves to reset clock = " + split[1]);
            auto reset_moves = String::to_number<size_t>(split[1]);
            auto time_split = String::split(split[2], ":");
            auto game_time = 0s;
            if(time_split.size() == 1)
            {
                log("game time = " + time_split[0] + " minutes");
                game_time = String::to_duration<std::chrono::minutes>(time_split[0]);
            }
            else
            {
                log("game time = " + time_split[0] + " minutes and " + time_split[1] + " seconds");
                game_time = String::to_duration<std::chrono::minutes>(time_split[0]) + String::to_duration<std::chrono::seconds>(time_split[1]);
            }

            log("increment = " + split[3]);
            auto increment = String::to_duration<Clock::seconds>(split[3]);
            clock = Clock(game_time, reset_moves, increment, Time_Reset_Method::ADDITION, Piece_Color::WHITE);
            need_to_set_time = false;;
        }
        else if(String::starts_with(command, "st "))
        {
            log("got time specs: " + command);
            auto split = String::split(command);
            auto time_per_move = String::to_duration<std::chrono::seconds>(split[1]);
            log("game time per move = " + std::to_string(time_per_move.count()));
            clock = Clock(time_per_move, 1, 0s, Time_Reset_Method::SET_TO_ORIGINAL, Piece_Color::WHITE);
            need_to_set_time = false;
        }
        else if(String::starts_with(command, "time "))
        {
            own_time_left = String::to_duration<centiseconds>(String::split(command, " ")[1]);
            need_to_set_time = true;
            log("Will set own time to " + std::to_string(own_time_left.count()) + " seconds.");
        }
        else if(String::starts_with(command, "otim "))
        {
            opponent_time_left = String::to_duration<centiseconds>(String::split(command, " ")[1]);
            need_to_set_time = true;
            log("Will set opponent's time to " + std::to_string(opponent_time_left.count()) + " seconds.");
        }
        else if(command == "undo")
        {
            undo_move(move_list, command, board, clock);
            setup_result = {};
        }
        else if(command == "remove")
        {
            if(undo_move(move_list, command, board, clock))
            {
                undo_move(move_list, command, board, clock);
            }
            setup_result = {};
        }
        else if(String::starts_with(command, "result "))
        {
            auto result = String::split(command).at(1);
            auto reason = String::extract_delimited_text(command, "{", "}");
            if(result == "1-0")
            {
                return Game_Result(Winner_Color::WHITE, reason, false);
            }
            else if(result == "0-1")
            {
                return Game_Result(Winner_Color::BLACK, reason, false);
            }
            else
            {
                return Game_Result(Winner_Color::NONE, reason, false);
            }
        }
    }

    if(need_to_set_time)
    {
        log("Setting own time (" + color_text(board.whose_turn()) + ") to " + std::to_string(own_time_left.count()) + " seconds.");
        clock.set_time(board.whose_turn(), own_time_left);
        log("Setting opponent's time (" + color_text(opposite(board.whose_turn())) + ") to " + std::to_string(opponent_time_left.count()) + " seconds.");
        clock.set_time(opposite(board.whose_turn()), opponent_time_left);
    }

    if( ! clock.is_running())
    {
        clock.start();
    }

    if(clock.running_for() != board.whose_turn())
    {
        clock.punch(board);
    }

    board.choose_move_at_leisure();

    return setup_result;
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
        log("Undoing move: " + move_list.back()->coordinates());
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

void CECP_Mediator::listen(const Board& board, Clock& clock)
{
    last_listening_command = std::async(std::launch::async, &CECP_Mediator::listener, this, std::ref(board), std::ref(clock));
}

Game_Result CECP_Mediator::handle_move(Board& board, const Move& move, std::vector<const Move*>& move_list, const Player&) const
{
    if(in_force_mode)
    {
        log("Ignoring move: " + move.coordinates());
        return {};
    }
    else
    {
        send_command("move " + move.coordinates());
        move_list.push_back(&move);
        auto result = board.submit_move(move);
        if(result.game_has_ended())
        {
            report_end_of_game(result);
        }
        return result;
    }
}

bool CECP_Mediator::pondering_allowed(const Board& board)
{
    if(thinking_on_opponent_time)
    {
        log("Starting to ponder");
        board.choose_move_at_leisure();
    }
    else
    {
        log("Skipping pondering");
    }
    return thinking_on_opponent_time;
}

std::string CECP_Mediator::receive_cecp_command(const Board& board, Clock& clock, bool while_listening)
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
        else if(command == "force")
        {
            log("Entering force mode");
            board.pick_move_now();
            clock.stop();
            in_force_mode = true;
        }
        else if(command == "post")
        {
            board.set_thinking_mode(Thinking_Output_Type::CECP);
            log("turning on thinking output for CECP");
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

std::string CECP_Mediator::listener(const Board& board, Clock& clock)
{
    while(true)
    {
        std::string command;
        try
        {
            command = receive_cecp_command(board, clock, true);
        }
        catch(const Game_Ended&)
        {
            board.pick_move_now();
            throw;
        }

        if(command == "?")
        {
            log("Forcing local AI to pick move and accepting it");
            board.pick_move_now();
        }
        else if(String::starts_with(command, "result "))
        {
            log("Stopped thinking about move by: " + command);
            board.pick_move_now();
            return command;
        }
        else
        {
            return command;
        }
    }
}

void CECP_Mediator::report_end_of_game(const Game_Result& result) const noexcept
{
    send_command(result.game_ending_annotation() + " {" + result.ending_reason() + "}");
}
