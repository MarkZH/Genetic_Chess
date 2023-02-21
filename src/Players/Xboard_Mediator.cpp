#include "Players/Xboard_Mediator.h"

#include <string>
#include <future>
#include <chrono>
using namespace std::chrono_literals;
#include <optional>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"

#include "Moves/Move.h"

#include "Players/Player.h"

#include "Utility/Exceptions.h"
#include "Utility/String.h"

Xboard_Mediator::Xboard_Mediator(const Player& local_player)
{
    const std::string expected = "protover 2";
    const auto command = receive_command();
    if(command == expected)
    {
        send_command("feature "
                     "usermove=1 "
                     "sigint=0 "
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
        throw std::runtime_error("Error in communicating with Xboard program. Expected \"" + expected + "\", got \"" + command + "\".");
    }
}

Game_Result Xboard_Mediator::setup_turn(Board& board, Clock& clock, std::vector<const Move*>& move_list, const Player& player)
{
    using centiseconds = std::chrono::duration<int, std::centi>;

    std::optional<Clock::seconds> own_time_left;
    std::optional<Clock::seconds> opponent_time_left;
    Game_Result setup_result;

    try
    {
        while(true)
        {
            const auto command = receive_xboard_command(clock, false);

            Player::pick_move_now(); // Stop pondering

            if(command.starts_with("ping "))
            {
                send_command("pong " + String::split(command).back());
            }
            else if(command == "go")
            {
                log("telling local AI to move at leisure and accepting move");
                in_force_mode = false;
                break;
            }
            else if(command == "new")
            {
                log("Setting board to standard start position and resetting clock");
                board = Board{};
                clock = Clock(clock.initial_time(), clock.moves_per_time_period(), clock.increment(Piece_Color::WHITE), clock.reset_mode());
                own_time_left.reset();
                opponent_time_left.reset();
                move_list.clear();
                setup_result = {};
                player.reset();
                in_force_mode = false;
            }
            else if(command.starts_with("name "))
            {
                const auto name = String::split(command, " ", 1).back();
                log("Getting other player's name: " + name);
                record_opponent_name(name);
            }
            else if(command.starts_with("setboard "))
            {
                try
                {
                    const auto fen = String::split(command, " ", 1).back();
                    const auto new_board = Board{fen};

                    // Handle GUIs that send the next board position
                    // instead of a move.
                    const auto new_move_list = board.derive_moves(new_board);
                    if(new_move_list.empty())
                    {
                        log("Rearranging board to: " + fen);
                        board = new_board;
                        move_list.clear();
                        setup_result = {};
                    }
                    else
                    {
                        for(auto move : new_move_list)
                        {
                            log("Derived move: " + move->coordinates());
                            setup_result = board.play_move(*move);
                            move_list.push_back(board.last_move());
                        }
                    }
                }
                catch(const std::invalid_argument&)
                {
                    send_error(command, "Bad FEN");
                }
            }
            else if(command.starts_with("usermove "))
            {
                const auto move = String::split(command).back();
                try
                {
                    log("Applying move: " + move);
                    setup_result = board.play_move(move);
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
                catch(const Illegal_Move&)
                {
                    send_command("Illegal move: " + move);
                }
            }
            else if(command.starts_with("level "))
            {
                log("got time specs: " + command);
                const auto split = String::split(command);

                log("moves to reset clock = " + split[1]);
                const auto reset_moves = String::to_number<size_t>(split[1]);
                const auto time_split = String::split(split[2], ":");
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
                const auto increment = String::to_duration<Clock::seconds>(split[3]);
                clock = Clock(game_time,
                              reset_moves,
                              increment,
                              Time_Reset_Method::ADDITION,
                              clock.game_start_date_and_time());
                own_time_left.reset();
                opponent_time_left.reset();
            }
            else if(command.starts_with("st "))
            {
                log("got time specs: " + command + " seconds");
                const auto split = String::split(command);
                const auto time_per_move = String::to_duration<Clock::seconds>(split[1]);
                log("game time per move = " + std::to_string(time_per_move.count()) + " seconds");
                clock = Clock(time_per_move,
                              1,
                              0.0s,
                              Time_Reset_Method::SET_TO_ORIGINAL,
                              clock.game_start_date_and_time());
                own_time_left.reset();
                opponent_time_left.reset();
            }
            else if(command.starts_with("time "))
            {
                own_time_left = String::to_duration<centiseconds>(String::split(command, " ")[1]);
                log("Will set own time to " + std::to_string(own_time_left->count()) + " seconds.");
            }
            else if(command.starts_with("otim "))
            {
                opponent_time_left = String::to_duration<centiseconds>(String::split(command, " ")[1]);
                log("Will set opponent's time to " + std::to_string(opponent_time_left->count()) + " seconds.");
            }
            else if(command == "undo")
            {
                undo_move(move_list, command, board, clock, player);
                setup_result = {};
            }
            else if(command == "remove")
            {
                if(undo_move(move_list, command, board, clock, player))
                {
                    undo_move(move_list, command, board, clock, player);
                }
                setup_result = {};
            }
            else if(command.starts_with("result "))
            {
                const auto result = String::split(command).at(1);
                const auto reason = String::extract_delimited_text(command, "{", "}");
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
            else if( ! usermove_prefix)
            {
                try
                {
                    log("Attempting to interpret as move: " + command);
                    setup_result = board.play_move(command);
                    log("Applied move: " + command);
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
                catch(const Illegal_Move&)
                {
                    log("Not a move, ignoring.");
                }
            }
        }
    }
    catch(const Game_Ended& game_ending_error)
    {
        return Game_Result(Winner_Color::NONE, game_ending_error.what(), true);
    }

    const auto own_time = own_time_left.value_or(clock.time_left(board.whose_turn()));
    log("Setting own time (" + color_text(board.whose_turn()) + ") to " + std::to_string(own_time.count()) + " seconds.");
    clock.set_time(board.whose_turn(), own_time);

    const auto opponent_time = opponent_time_left.value_or(clock.time_left(opposite(board.whose_turn())));
    log("Setting opponent's time (" + color_text(opposite(board.whose_turn())) + ") to " + std::to_string(opponent_time.count()) + " seconds.");
    clock.set_time(opposite(board.whose_turn()), opponent_time);

    Player::choose_move_at_leisure();
    return setup_result;
}

bool Xboard_Mediator::undo_move(std::vector<const Move*>& move_list, const std::string& command, Board& board, Clock& clock, const Player& player)
{
    if(move_list.empty())
    {
        send_error(command, "no moves to undo");
        return false;
    }
    else
    {
        log("Undoing move: " + move_list.back()->coordinates());
        player.undo_move(board.last_move());
        move_list.pop_back();
        auto new_board = Board(board.original_fen());
        for(auto move : move_list)
        {
            new_board.play_move(*move);
        }
        board = new_board;
        clock.unpunch();
        return true;
    }
}

Game_Result Xboard_Mediator::handle_move(Board& board, const Move& move, std::vector<const Move*>& move_list) const
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
        const auto result = board.play_move(move);
        if(result.game_has_ended())
        {
            report_end_of_game(result);
        }
        return result;
    }
}

std::string Xboard_Mediator::receive_xboard_command(Clock& clock, bool while_listening)
{
    while(true)
    {
        const auto command = get_last_command(while_listening);

        if(command == "force")
        {
            log("Entering force mode");
            Player::pick_move_now();
            clock.stop();
            in_force_mode = true;
        }
        else if(command == "post")
        {
            Player::set_thinking_mode(Thinking_Output_Type::XBOARD);
            log("turning on thinking output for Xboard");
        }
        else if(command == "nopost")
        {
            Player::set_thinking_mode(Thinking_Output_Type::NO_THINKING);
            log("turning off thinking output for Xboard");
        }
        else if(command == "rejected usermove")
        {
            usermove_prefix = false;
            log("Moves will not be preceded by \"usermove\"");
        }
        else
        {
            return command;
        }
    }
}

void Xboard_Mediator::send_error(const std::string& command, const std::string& reason) const noexcept
{
    send_command("Error (" + reason + "): " + command);
}

std::string Xboard_Mediator::listener(Clock& clock)
{
    while(true)
    {
        try
        {
            const auto command = receive_xboard_command(clock, true);

            if(command == "?")
            {
                log("Forcing local AI to pick move and accepting it");
                Player::pick_move_now();
            }
            else if(command.starts_with("result "))
            {
                log("Stopped thinking about move by: " + command);
                Player::pick_move_now();
                return command;
            }
            else
            {
                return command;
            }
        }
        catch(const Game_Ended&)
        {
            Player::pick_move_now();
            throw;
        }
    }
}

void Xboard_Mediator::report_end_of_game(const Game_Result& result) const noexcept
{
    send_command(result.game_ending_annotation() + " {" + result.ending_reason() + "}");
}
