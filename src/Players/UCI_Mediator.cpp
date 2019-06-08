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

UCI_Mediator::UCI_Mediator(const Player& player)
{
    send_command("id name " + player.name());
    send_command("id author " + player.author());
    send_command("uciok");
}

void UCI_Mediator::setup_turn(Board& board, Clock& clock)
{
    while(true)
    {
        auto command = receive_uci_command(board, clock, false);
        if(command == "ucinewgame")
        {
            log("stopping thinking and clocks");
            board.pick_move_now();
            clock.stop();
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

            auto moves_iter = std::find(parse.begin(), parse.end(), "moves");
            if(moves_iter != parse.end())
            {
                std::for_each(std::next(moves_iter), parse.end(),
                              [&board](const auto& move)
                              {
                                  board.submit_move(board.create_move(move));
                              });
                log("All moves applied");
            }

            log("Board ready for play");
            board.set_thinking_mode(UCI);
        }
        else if(String::starts_with(command, "go"))
        {
            if(board.whose_turn() == WHITE)
            {
                indent = "\t\t";
            }
            else
            {
                indent = "\t\t\t";
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
                    auto moves_to_reset = String::string_to_size_t(go_parse.at(++i));
                    log("Next time control in " + std::to_string(moves_to_reset));
                    clock.set_next_time_reset(moves_to_reset);
                }
                else if(option == "movetime")
                {
                    clock.set_time(board.whose_turn(), new_time);
                    clock.set_next_time_reset(1);
                }
                else
                {
                    log("Ignoring go command: " + option);
                }
            }

            log("Telling AI to choose a move at leisure");
            board.choose_move_at_leisure();
            return;
        }
    }
}

void UCI_Mediator::listen(Board& board, Clock& clock)
{
    last_listening_result = std::async(std::launch::async, &UCI_Mediator::listener, this, std::ref(board), std::ref(clock));
}

void UCI_Mediator::handle_move(Board& board, const Move& move)
{
    board.submit_move(move);
    send_command("bestmove " + move.coordinate_move());
}

bool UCI_Mediator::pondering_allowed() const
{
    return true;
}

std::string UCI_Mediator::listener(Board& board, Clock& clock)
{
    return receive_uci_command(board, clock, true);
}

std::string UCI_Mediator::receive_uci_command(Board& board, Clock& clock, bool while_listening)
{
    clock.do_not_stop_game();

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
