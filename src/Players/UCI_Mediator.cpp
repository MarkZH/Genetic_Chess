#include "Players/UCI_Mediator.h"

#include <string>
#include <future>
#include <numeric>
#include <algorithm>

#include "Players/Player.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility/String.h"

#include "Exceptions/Game_Ended.h"

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
            return Game_Result(NONE, game_ending_error.what());
        }

        if(command == "ucinewgame")
        {
            log("stopping thinking and clocks");
            board.pick_move_now();
            clock = {};
            move_list.clear();
            player.reset();
            setup_result = {};
        }
        else if(String::starts_with(command, "setoption name UCI_Opponent value "))
        {
            // command has 8 fields requiring 7 cuts to get name
            auto name = String::split(command, " ", 7).back();
            player.set_opponent_name(name);
            log("Opponent's name: " + name);
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
            if( ! clock.is_running())
            {
                clock.start();
            }

            if(clock.running_for() != board.whose_turn())
            {
                clock.punch(board);
            }

            auto go_parse = String::split(command);
            for(size_t i = 1; i < go_parse.size(); ++i)
            {
                auto option = go_parse.at(i);
                double new_time = 0.0;
                if(String::ends_with(option, "time") || String::ends_with(option, "inc"))
                {
                    // All times received are in milliseconds
                    new_time = std::stod(go_parse.at(++i))/1000;
                }

                if(option == "wtime")
                {
                    log("Setting White's time to " + std::to_string(new_time));
                    clock.set_time(WHITE, new_time);
                }
                else if(option == "btime")
                {
                    log("Setting Black's time to " + std::to_string(new_time));
                    clock.set_time(BLACK, new_time);
                }
                else if(option == "winc")
                {
                    log("Setting White's increment time to " + std::to_string(new_time));
                    clock.set_increment(WHITE, new_time);
                }
                else if(option == "binc")
                {
                    log("Setting Black's increment time to " + std::to_string(new_time));
                    clock.set_increment(BLACK, new_time);
                }
                else if(option == "movestogo")
                {
                    auto moves_to_reset = String::string_to_number<size_t>(go_parse.at(++i));
                    log("Next time control in " + std::to_string(moves_to_reset));
                    clock.set_next_time_reset(moves_to_reset);
                }
                else if(option == "movetime")
                {
                    log("Setting clock to " + std::to_string(new_time) + " seconds per move");
                    clock = Clock(new_time, 1, 0.0, board.whose_turn(), clock.game_start_date_and_time());
                    clock.start();
                }
                else
                {
                    log("Ignoring go command: " + option);
                }
            }

            log("Telling AI to choose a move at leisure");
            board.choose_move_at_leisure();
            return setup_result;
        }
    }
}

void UCI_Mediator::listen(Board& board, Clock&)
{
    last_listening_result = std::async(std::launch::async, &UCI_Mediator::listener, this, std::ref(board));
}

Game_Result UCI_Mediator::handle_move(Board& board,
                                      const Move& move,
                                      std::vector<const Move*>& move_list,
                                      const Player& player) const
{
    auto command = "bestmove " + move.coordinate_move();
    auto expected_move = player.expected_response();
    if(expected_move)
    {
        command += " ponder " + expected_move->coordinate_move();
    }
    send_command(command);
    move_list.push_back(&move);
    return board.submit_move(move);
}

bool UCI_Mediator::pondering_allowed() const
{
    return true;
}

std::string UCI_Mediator::listener(Board& board)
{
    return receive_uci_command(board, true);
}

std::string UCI_Mediator::receive_uci_command(Board& board, bool while_listening)
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
