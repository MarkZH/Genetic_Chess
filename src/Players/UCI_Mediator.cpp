#include "Players/UCI_Mediator.h"

#include <string>
#include <future>
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
    send_command("option name UCI_Opponent type string");
    send_command("uciok");
}

Game_Result UCI_Mediator::setup_turn(Board& board, Clock& clock, std::vector<const Move*>& move_list, const Player& player)
{
    Game_Result setup_result;

    try
    {
        while(true)
        {
            const auto command = receive_uci_command(false);

            Player::pick_move_now(); // Stop pondering

            if(command == "ucinewgame")
            {
                log("stopping thinking and clocks");
                clock = {};
                move_list.clear();
                player.reset();
                setup_result = {};
            }
            else if(command.starts_with("setoption name UCI_Opponent value "))
            {
                // command has 8 fields requiring 7 cuts to get name
                const auto opponent_split = String::split(command, " ", 7);
                if(opponent_split.size() != 8)
                {
                    log("Malformed UCI_Opponent line: " + command);
                }
                else
                {
                    const auto& title = opponent_split[4];
                    log("Opponent title: " + title);
                    log("Opponent rating: " + opponent_split[5]);
                    log("Opponent type: " + opponent_split[6]);
                    const auto& name = opponent_split[7];
                    record_opponent_name(name, title);
                    log("Opponent's name: " + name);
                }
            }
            else if(command.starts_with("position "))
            {
                const auto parse = String::split(command);
                if(parse.at(1) == "startpos")
                {
                    board = Board();
                }
                else if(parse.at(1) == "fen")
                {
                    const auto fen_begin = std::next(parse.begin(), 2);
                    const auto fen_end = std::next(fen_begin, 6);
                    board = Board(String::join(fen_begin, fen_end, " "));
                }

                move_list.clear();
                const auto moves_iter = std::ranges::find(parse, "moves");
                if(moves_iter != parse.end())
                {
                    std::transform(std::next(moves_iter), parse.end(), std::back_inserter(move_list),
                                   [&board, &setup_result](const auto& move)
                                   {
                                       setup_result = board.play_move(move);
                                       return board.last_move();
                                   });
                    log("All moves applied");
                    if(setup_result.game_has_ended())
                    {
                        return setup_result;
                    }
                }

                log("Board ready for play");
                Player::set_thinking_mode(Thinking_Output_Type::UCI);
            }
            else if(command.starts_with("go "))
            {
                const auto mode = clock.reset_mode();
                auto new_mode = mode;
                auto wtime = clock.time_left(Piece_Color::WHITE);
                auto btime = clock.time_left(Piece_Color::BLACK);
                auto winc = clock.increment(Piece_Color::WHITE);
                auto binc = clock.increment(Piece_Color::BLACK);
                auto movestogo = size_t{0};
                auto movetime = clock.initial_time();
                auto search_moves = std::vector<const Move*>();

                std::string parameter;
                for(const auto& token : String::split(command))
                {
                    if(token == "go")
                    {
                        continue;
                    }

                    if(parameter.empty())
                    {
                        parameter = token;
                        continue;
                    }

                    if(parameter == "searchmoves")
                    {
                        if(board.is_legal_move(token))
                        {
                            search_moves.push_back(&board.interpret_move(token));
                        }
                        else
                        {
                            parameter = token;
                        }
                        continue;
                    }

                    const auto number = String::to_number<int>(token);
                    if(parameter == "wtime")
                    {
                        log("Setting White's time to " + std::to_string(number) + " ms");
                        wtime = std::chrono::milliseconds{number};
                        new_mode = Time_Reset_Method::ADDITION;
                    }
                    else if(parameter == "btime")
                    {
                        log("Setting Black's time to " + std::to_string(number) + " ms");
                        btime = std::chrono::milliseconds{number};
                        new_mode = Time_Reset_Method::ADDITION;
                    }
                    else if(parameter == "winc")
                    {
                        log("Setting White's increment time to " + std::to_string(number) + " ms");
                        winc = std::chrono::milliseconds{number};
                        new_mode = Time_Reset_Method::ADDITION;
                    }
                    else if(parameter == "binc")
                    {
                        log("Setting Black's increment time to " + std::to_string(number) + " ms");
                        binc = std::chrono::milliseconds{number};
                        new_mode = Time_Reset_Method::ADDITION;
                    }
                    else if(parameter == "movestogo")
                    {
                        log("Next time control in " + std::to_string(number) + " moves");
                        movestogo = number;
                        new_mode = Time_Reset_Method::ADDITION;
                    }
                    else if(parameter == "movetime")
                    {
                        log("Setting clock to " + std::to_string(number) + " ms per move");
                        movetime = std::chrono::milliseconds{number};
                        new_mode = Time_Reset_Method::SET_TO_ORIGINAL;
                    }
                    else
                    {
                        log("Ignoring go command: " + parameter);
                    }

                    parameter.clear();
                }

                if(new_mode != mode || ! clock.is_running())
                {
                    if(new_mode == Time_Reset_Method::ADDITION)
                    {
                        clock = Clock(board.whose_turn() == Piece_Color::WHITE ? wtime : btime,
                                      movestogo,
                                      board.whose_turn() == Piece_Color::WHITE ? winc : binc,
                                      new_mode,
                                      clock.game_start_date_and_time());
                    }
                    else
                    {
                        clock = Clock(movetime,
                                      1,
                                      0.0s,
                                      new_mode,
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
                        clock.set_next_time_reset(board.whose_turn(), movestogo);
                    }
                    else
                    {
                        clock.set_time(board.whose_turn(), movetime);
                    }
                }

                if( ! search_moves.empty())
                {
                    board.legal_moves_cache = search_moves;
                }

                log("Telling AI to choose a move at leisure");
                Player::choose_move_at_leisure();
                return setup_result;
            }
        }
    }
    catch(const Game_Ended& game_ending_error)
    {
        return Game_Result(Winner_Color::NONE, game_ending_error.what(), true);
    }
}

Game_Result UCI_Mediator::handle_move(Board& board,
                                      const Move& move,
                                      std::vector<const Move*>& move_list) const
{
    send_command("bestmove " + move.coordinates());
    move_list.push_back(&move);
    return board.play_move(move);
}

std::string UCI_Mediator::listener(Clock&)
{
    try
    {
        return receive_uci_command(true);
    }
    catch(const Game_Ended&)
    {
        Player::pick_move_now();
        throw;
    }
}

std::string UCI_Mediator::receive_uci_command(bool while_listening)
{
    while(true)
    {
        const auto command = get_last_command(while_listening);

        if(command == "isready")
        {
            send_command("readyok");
        }
        else if(command == "stop")
        {
            log("Stopping local AI thinking");
            Player::pick_move_now();
        }
        else
        {
            return command;
        }
    }
}
