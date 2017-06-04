#include "Game/Game.h"

#include <fstream>
#include <string>
#include <mutex>

#include "Players/Player.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Moves/Complete_Move.h"
#include "Exceptions/Game_Ending_Exception.h"
#include "Exceptions/Out_Of_Time_Exception.h"

#include "Players/Thinking.h"

#include "Utility.h"

// Play single game, return color of winner
Color play_game(const Player& white,
                const Player& black,
                double time_in_seconds,
                size_t moves_to_reset,
                double increment_seconds,
                const std::string& pgn_file_name)
{
    Board board;

    white.initial_board_setup(board);
    black.initial_board_setup(board);

    return play_game_with_board(white,
                                black,
                                time_in_seconds,
                                moves_to_reset,
                                increment_seconds,
                                pgn_file_name, board);
}

Color play_game_with_board(const Player& white,
                           const Player& black,
                           double time_in_seconds,
                           size_t moves_to_reset,
                           double increment_seconds,
                           const std::string& pgn_file_name,
                           Board& board)
{
    static std::mutex write_lock;
    Clock game_clock(time_in_seconds, moves_to_reset, increment_seconds);

    try
    {
        game_clock.start();

        while(true)
        {
			auto& player = board.whose_turn() == WHITE ? white : black;
            auto move_chosen = player.choose_move(board, game_clock);
            game_clock.punch();
            board.submit_move(move_chosen);
        }
    }
    catch(const Game_Ending_Exception& end_game)
    {
		// for Outside_Players communicating with xboard and the like
        if(board.whose_turn() == WHITE)
        {
            white.process_game_ending(end_game, board);
        }
        else
        {
            black.process_game_ending(end_game, board);
        }

        std::lock_guard<std::mutex> write_lock_guard(write_lock);

        board.print_game_record(&white,
                                &black,
                                pgn_file_name,
                                end_game.winner(),
                                end_game.what());

        if(game_clock.is_running())
        {
            std::ofstream(pgn_file_name, std::ios::app)
                << "{ Initial time: " << time_in_seconds << " }\n"
                << "{ Moves to reset clocks: " << moves_to_reset << " }\n"
                << "{ Time increment: " << increment_seconds << " }\n"
                << "{ Time left: White: " << game_clock.time_left(WHITE) << " }\n"
                << "{            Black: " << game_clock.time_left(BLACK) << " }\n\n"
                << std::endl;
        }

        return end_game.winner();
    }
    catch(const std::exception& error)
    {
        std::lock_guard<std::mutex> write_lock_guard(write_lock);
        board.ascii_draw(WHITE);
        board.print_game_record(&white, &black, pgn_file_name, NONE, error.what());
        throw;
    }
}
