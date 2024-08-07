#include "Game/Game.h"

#include <string>
#include <vector>
#include <csignal>
#include <iostream>

#include "Players/Player.h"
#include "Players/Genetic_AI.h"
#include "Players/Random_AI.h"
#include "Players/Proxy_Player.h"
#include "Players/Outside_Communicator.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Game/Move.h"

#include "Utility/String.h"
#include "Utility/Main_Tools.h"

Game_Result play_game(Board board,
                      Clock game_clock,
                      const Player& white,
                      const Player& black,
                      const std::string& event_name,
                      const std::string& location,
                      const std::string& pgn_file_name,
                      const bool print_board) noexcept
{
    std::vector<const Move*> game_record;
    Game_Result result;

    game_clock.start(board.whose_turn());

    while( ! result.game_has_ended())
    {
        const auto& player = board.whose_turn() == Piece_Color::WHITE ? white : black;
        const auto& move_chosen = player.choose_move(board, game_clock);

        if(Player::thinking_mode() != Thinking_Output_Type::NO_THINKING)
        {
            std::cout << player.name() << " chose " << move_chosen.algebraic(board) << '\n';
        }

        result = game_clock.punch(board);
        if( ! result.game_has_ended())
        {
            result = board.play_move(move_chosen);
            game_record.push_back(&move_chosen);
            if(print_board)
            {
                board.cli_print_game(white, black, game_clock);
            }
        }
    }

    game_clock.stop();
    board.print_game_record(game_record,
                            white,
                            black,
                            pgn_file_name,
                            result,
                            game_clock,
                            event_name,
                            location);
    return result;
}

void play_game_with_outsider(const Player& player,
                             const std::string& event_name,
                             const std::string& location,
                             const std::string& game_file_name,
                             const bool enable_logging)
{
    const auto outsider = connect_to_outside(player, enable_logging);

    signal(SIGINT, SIG_IGN);

    Board board;
    Clock clock;
    Game_Result game_result;
    std::vector<const Move*> game_record;
    auto player_color = Piece_Color::BLACK;
    auto print_game_record = false;

    while( ! game_result.exit_program())
    {
        // A do-while loop is used here instead of while( !  game_result.game_has_ended())
        // because the "quit" command is only received after another entry into the loop
        // after a game has finished. A normal while loop would keep spinning because the
        // Game_Result that exited the inner loop would never be updated by a call to
        // setup_turn(), so the "quit" command would never be received.
        do
        {
            game_result = outsider->setup_turn(board, clock, game_record, player);
            clock.start(board.whose_turn());
            if(game_result.game_has_ended())
            {
                break;
            }
            outsider->listen(clock);
            print_game_record = true;

            player_color = board.whose_turn();
            const auto& chosen_move = player.choose_move(board, clock);
            clock.punch(board);

            game_result = outsider->handle_move(board, chosen_move, game_record);
        } while( ! game_result.game_has_ended());

        outsider->log("Game ended with: " + game_result.ending_reason());
        if(print_game_record && ! game_file_name.empty())
        {
            clock.stop();
            const auto opponent_proxy = outsider->create_proxy_player();
            const Player& white = (player_color == Piece_Color::WHITE ? player : opponent_proxy);
            const Player& black = (player_color == Piece_Color::BLACK ? player : opponent_proxy);
            board.print_game_record(game_record,
                                    white, black,
                                    game_file_name,
                                    game_result,
                                    clock,
                                    event_name,
                                    location);
            print_game_record = false;
        }
    }
}

void start_game(const std::vector<std::tuple<std::string, std::vector<std::string>>>& options)
{
    // Use pointers since each player could be Genetic, Random, etc.
    std::unique_ptr<Player> white;
    std::unique_ptr<Player> black;

    Clock::seconds game_time{};
    size_t moves_per_reset = 0;
    Clock::seconds increment_time{};
    Board board;
    std::string game_file_name;
    std::string event_name;
    std::string location;
    auto thinking_output = Thinking_Output_Type::NO_THINKING;
    auto print_board = false;
    auto enable_logging = false;

    for(const auto& [opt, values] : options)
    {
        std::unique_ptr<Player> latest;
        if(opt == "-random")
        {
            latest = std::make_unique<Random_AI>();
        }
        else if(opt == "-genetic")
        {
            Main_Tools::argument_assert( ! values.empty(), "Genome file needed for player");
            std::string file_name = values[0];

            try
            {
                const auto id = values.size() > 1 ? values[1] : std::string{};
                latest = std::make_unique<Genetic_AI>(file_name, String::to_number<int>(id));
            }
            catch(const std::invalid_argument&) // Could not convert id to an int.
            {
                latest = std::make_unique<Genetic_AI>(file_name, find_last_id(file_name));
            }
        }
        else if(opt == "-time")
        {
            Main_Tools::argument_assert( ! values.empty(), opt + " requires a numeric parameter.");
            game_time = String::to_duration<Clock::seconds>(values[0]);
        }
        else if(opt == "-reset-moves")
        {
            Main_Tools::argument_assert( ! values.empty(), opt + " requires a whole number parameter.");
            moves_per_reset = String::to_number<size_t>(values[0]);
        }
        else if(opt == "-increment-time")
        {
            Main_Tools::argument_assert( ! values.empty(), opt + " requires a numeric parameter.");
            increment_time = String::to_duration<Clock::seconds>(values[0]);
        }
        else if(opt == "-board")
        {
            Main_Tools::argument_assert( ! values.empty(), opt + " requires a quoted FEN text parameter.");
            board = Board(values[0]);
        }
        else if(opt == "-game-file")
        {
            Main_Tools::argument_assert( ! values.empty(), opt + " requires a file name.");
            game_file_name = values[0];
        }
        else if(opt == "-event")
        {
            Main_Tools::argument_assert( ! values.empty(), opt + " requires a text parameter.");
            event_name = values[0];
        }
        else if(opt == "-location")
        {
            Main_Tools::argument_assert( ! values.empty(), opt + " requires a text parameter.");
            location = values[0];
        }
        else if(opt == "-xboard")
        {
            thinking_output = Thinking_Output_Type::XBOARD;
        }
        else if(opt == "-uci")
        {
            thinking_output = Thinking_Output_Type::UCI;
        }
        else if(opt == "-show-board")
        {
            print_board = true;
        }
        else if(opt == "-log-comms")
        {
            enable_logging = true;
        }
        else
        {
            throw std::invalid_argument("Invalid or incomplete game option: " + opt);
        }

        if(latest)
        {
            if(!white)
            {
                white = std::move(latest);
            }
            else if(!black)
            {
                black = std::move(latest);
            }
            else
            {
                throw std::invalid_argument("More than two players specified.");
            }
        }
    }

    if(!white)
    {
        throw std::invalid_argument("At least one player must be specified.");
    }

    if(!black)
    {
        play_game_with_outsider(*white, event_name, location, game_file_name, enable_logging);
    }
    else
    {
        Player::set_thinking_mode(thinking_output);
        play_game(board,
                  Clock(game_time, moves_per_reset, increment_time, Time_Reset_Method::ADDITION),
                  *white, *black,
                  event_name,
                  location,
                  game_file_name,
                  print_board);
    }
}
