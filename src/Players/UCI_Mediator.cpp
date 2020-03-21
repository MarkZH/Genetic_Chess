#include "Players/UCI_Mediator.h"

#include <string>
#include <future>
#include <numeric>
#include <algorithm>
#include <chrono>
using namespace std::chrono_literals;

#include "Players/Player.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility/String.h"
#include "Utility/Exceptions.h"

UCI_Mediator::UCI_Mediator(const Player& player)
{
    send_command("id name " + player.name());
    send_command("id author " + player.author());
    send_command("option name UCI_Opponent type string default <empty>");
    send_command("uciok");
}

Game_Result UCI_Mediator::setup_turn(Board& board, Clock& clock, std::vector<const Move*>& move_list, const Player& player)
{
    Game_Result setup_result;

    while(true)
    {
        std::string command;
        try
        {
            command = receive_uci_command(board, false);
        }
        catch(const Game_Ended& game_ending_error)
        {
            return Game_Result(Winner_Color::NONE, game_ending_error.what(), true);
        }

        board.pick_move_now(); // Stop pondering

        if(command == "ucinewgame")
        {
            log("stopping thinking and clocks");
            clock = {};
            move_list.clear();
            player.reset();
            setup_result = {};
        }
        else if(String::starts_with(command, "setoption name UCI_Opponent value "))
        {
            // command has 8 fields requiring 7 cuts to get name
            auto opponent_split = String::split(command, " ", 7);
            if(opponent_split.size() != 8)
            {
                log("Malformed UCI_Opponent line: " + command);
            }
            else
            {
                auto title = opponent_split[4];
                log("Opponent title: " + title);
                if(title == "none")
                {
                    title.clear();
                }
                else
                {
                    title = title + " ";
                }

                auto rating = opponent_split[5];
                log("Opponent rating: " + rating);

                auto type = opponent_split[6];
                log("Opponent type: " + type);

                auto name = opponent_split[7];
                player.set_opponent_name(title + name);
                log("Opponent's name: " + name);
            }
        }
        else if(String::starts_with(command, "position "))
        {
            auto parse = String::split(command);
            if(parse.at(1) == "startpos")
            {
                board = Board();
            }
            else if(parse.at(1) == "fen")
            {
                auto fen = std::accumulate(std::next(parse.begin(), 3), std::next(parse.begin(), 8),
                                           parse.at(2),
                                           [](const auto& so_far, const auto& next)
                                           {
                                               return so_far + " " + next;
                                           });
                board = Board(fen);
            }

            move_list.clear();
            auto moves_iter = std::find(parse.begin(), parse.end(), "moves");
            if(moves_iter != parse.end())
            {
                std::for_each(std::next(moves_iter), parse.end(),
                              [&board, &move_list, &setup_result](const auto& move)
                              {
                                  setup_result = board.submit_move(move);
                                  move_list.push_back(board.last_move());
                              });
                log("All moves applied");
            }

            log("Board ready for play");
            board.set_thinking_mode(Thinking_Output_Type::UCI);
        }
        else if(String::starts_with(command, "go "))
        {
            const auto mode = clock.reset_mode();
            auto new_mode = mode;
            auto wtime = clock.time_left(Piece_Color::WHITE);
            auto btime = clock.time_left(Piece_Color::BLACK);
            auto winc = clock.increment(Piece_Color::WHITE);
            auto binc = clock.increment(Piece_Color::BLACK);
            auto movestogo = size_t{0};
            auto movetime = clock.initial_time();

            std::string previous_option;
            for(const auto& option : String::split(command))
            {
                if(option == "go")
                {
                    continue;
                }

                if(previous_option.empty())
                {
                    previous_option = option;
                    continue;
                }

                auto number = std::stoi(option);
                if(previous_option == "wtime")
                {
                    log("Setting White's time to " + std::to_string(number) + " ms");
                    wtime = std::chrono::milliseconds{number};
                    new_mode = Time_Reset_Method::ADDITION;
                }
                else if(previous_option == "btime")
                {
                    log("Setting Black's time to " + std::to_string(number) + " ms");
                    btime = std::chrono::milliseconds{number};
                    new_mode = Time_Reset_Method::ADDITION;
                }
                else if(previous_option == "winc")
                {
                    log("Setting White's increment time to " + std::to_string(number) + " ms");
                    winc = std::chrono::milliseconds{number};
                    new_mode = Time_Reset_Method::ADDITION;
                }
                else if(previous_option == "binc")
                {
                    log("Setting Black's increment time to " + std::to_string(number) + " ms");
                    binc = std::chrono::milliseconds{number};
                    new_mode = Time_Reset_Method::ADDITION;
                }
                else if(previous_option == "movestogo")
                {
                    log("Next time control in " + std::to_string(number) + " moves");
                    movestogo = number;
                    new_mode = Time_Reset_Method::ADDITION;
                }
                else if(previous_option == "movetime")
                {
                    log("Setting clock to " + std::to_string(number) + " ms per move");
                    movetime = std::chrono::milliseconds{number};
                    new_mode = Time_Reset_Method::SET_TO_ORIGINAL;
                }
                else
                {
                    log("Ignoring go command: " + previous_option);
                }

                previous_option.clear();
            }

            if(new_mode != mode || ! clock.is_in_use())
            {
                if(new_mode == Time_Reset_Method::ADDITION)
                {
                    clock = Clock(board.whose_turn() == Piece_Color::WHITE ? wtime : btime,
                                  movestogo,
                                  board.whose_turn() == Piece_Color::WHITE ? winc : binc,
                                  new_mode,
                                  board.whose_turn(),
                                  clock.game_start_date_and_time());
                }
                else
                {
                    clock = Clock(movetime,
                                  1,
                                  0s,
                                  new_mode,
                                  board.whose_turn(),
                                  clock.game_start_date_and_time());
                }
            }
            else
            {
                if(new_mode == Time_Reset_Method::ADDITION)
                {
                    clock.set_time(Piece_Color::WHITE, wtime);
                    clock.set_time(Piece_Color::BLACK, btime);
                    clock.set_increment(Piece_Color::WHITE, winc);
                    clock.set_increment(Piece_Color::BLACK, binc);
                    clock.set_next_time_reset(movestogo);
                }
                else
                {
                    clock.set_time(board.whose_turn(), movetime);
                }
            }

            if( ! clock.is_running())
            {
                clock.start();
            }

            if(clock.running_for() != board.whose_turn())
            {
                clock.punch(board);
            }

            log("Telling AI to choose a move at leisure");
            board.choose_move_at_leisure();
            return setup_result;
        }
    }
}

void UCI_Mediator::listen(const Board& board, Clock&)
{
    last_listening_result = std::async(std::launch::async, &UCI_Mediator::listener, this, std::ref(board));
}

Game_Result UCI_Mediator::handle_move(Board& board,
                                      const Move& move,
                                      std::vector<const Move*>& move_list,
                                      const Player& player) const
{
    auto command = "bestmove " + move.coordinates();
    auto expected_move = player.expected_response();
    if(expected_move)
    {
        command += " ponder " + expected_move->coordinates();
    }
    send_command(command);
    move_list.push_back(&move);
    return board.submit_move(move);
}

bool UCI_Mediator::pondering_allowed(const Board& board)
{
    auto command = receive_uci_command(board, false);
    if(String::starts_with(command, "go ") && String::contains(command, "ponder"))
    {
        log("Starting to ponder");
        board.choose_move_at_leisure();
        return true;
    }
    else
    {
        log("Skipping pondering");
        last_listening_result = std::async(std::launch::async, [command]() { return command; });
        return false;
    }
}

std::string UCI_Mediator::listener(const Board& board)
{
    try
    {
        return receive_uci_command(board, true);
    }
    catch(const Game_Ended&)
    {
        board.pick_move_now();
        throw;
    }
}

std::string UCI_Mediator::receive_uci_command(const Board& board, bool while_listening)
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
            command = last_listening_result.valid() ? last_listening_result.get() : receive_command();
        }

        if(command == "isready")
        {
            send_command("readyok");
        }
        else if(command == "stop")
        {
            log("Stopping local AI thinking");
            board.pick_move_now();
        }
        else
        {
            return command;
        }
    }
}
