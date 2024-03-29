#include "Game/Game.h"

#include <string>
#include <vector>
#include <csignal>
#include <iostream>

#include "Players/Player.h"
#include "Players/Proxy_Player.h"
#include "Players/Outside_Communicator.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Game/Move.h"

#include "Utility/String.h"

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
