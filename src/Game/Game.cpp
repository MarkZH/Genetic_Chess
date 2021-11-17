#include "Game/Game.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <csignal>
#include <cassert>
#include <iostream>

#include "Players/Player.h"
#include "Players/Proxy_Player.h"
#include "Players/Outside_Communicator.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility/String.h"

Game_Result play_game(Board board,
                      Clock game_clock,
                      const Player& white,
                      const Player& black,
                      const std::string& event_name,
                      const std::string& location,
                      const std::string& pgn_file_name)
{
    if(game_clock.is_in_use() && board.whose_turn() != game_clock.running_for())
    {
        throw std::runtime_error("Board and Clock do not agree on whose turn it is.");
    }

    std::vector<const Move*> game_record;
    Game_Result result;

    game_clock.start();

    while( ! result.game_has_ended())
    {
        const auto& player = board.whose_turn() == Piece_Color::WHITE ? white : black;
        const auto& move_chosen = player.choose_move(board, game_clock);

        result = game_clock.punch(board);
        if( ! result.game_has_ended())
        {
            std::cout << color_text(board.whose_turn()) << " plays: " << move_chosen.algebraic(board) << std::endl;
            result = board.play_move(move_chosen);
            game_record.push_back(&move_chosen);
        }
    }

    game_clock.stop();
    std::cout << std::endl;
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
                             const std::string& game_file_name)
{
    const auto outsider = connect_to_outside(player);

    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    Board board;
    Clock clock;
    Game_Result game_result;
    std::vector<const Move*> game_record;
    auto player_color = Piece_Color::BLACK;
    auto print_game_record = true;

    while(true)
    {
        while(true)
        {
            game_result = outsider->setup_turn(board, clock, game_record, player);
            if(game_result.game_has_ended())
            {
                break;
            }
            outsider->listen(board, clock);
            print_game_record = true;

            player_color = board.whose_turn();
            const auto& chosen_move = player.choose_move(board, clock);
            clock.punch(board);

            game_result = outsider->handle_move(board, chosen_move, game_record);
            if(game_result.game_has_ended())
            {
                break;
            }
        }

        outsider->log("Game ended with: " + game_result.ending_reason());
        if(print_game_record && ! game_file_name.empty())
        {
            clock.stop();
            const auto opponent_proxy = outsider->create_proxy_player();
            const Player& white = (player_color == Piece_Color::WHITE ? player : opponent_proxy);
            const Player& black = (player_color == Piece_Color::BLACK ? player : opponent_proxy);
            board.print_game_record(game_record,
                                    white, black,
                                    String::add_to_file_name(game_file_name, "-" + color_text(player_color)),
                                    game_result,
                                    clock,
                                    event_name,
                                    location);
            print_game_record = false;
        }

        if(game_result.exit_program())
        {
            return;
        }
    }
}
