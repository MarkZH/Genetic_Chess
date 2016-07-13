#include "Game/Game.h"

#include <fstream>
#include <string>
#include <chrono>
#include <mutex>

#include "Players/Player.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Moves/Move.h"
#include "Exceptions/Game_Ending_Exception.h"
#include "Exceptions/Out_Of_Time_Exception.h"

// Play single game, return color of winner
Color play_game(const Player& white,
                const Player& black,
                int time_in_seconds,
                int moves_to_reset,
                const std::string& pgn_file_name)
{
    Board board;
    Clock game_clock(time_in_seconds, moves_to_reset);

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
        white.process_game_ending(end_game);
        black.process_game_ending(end_game);

        static std::mutex write_lock;
        std::lock_guard<std::mutex> write_lock_guard(write_lock);
        board.print_game_record(white.name(), black.name(), pgn_file_name, end_game.what());
        if(game_clock.is_running())
        {
            std::ofstream(pgn_file_name, std::ios::app)
                << "; Initial time: " << time_in_seconds << "\n"
                << "; Moves to reset clocks: " << moves_to_reset << "\n"
                << "; Time left: White: " << game_clock.time_left(WHITE) << "\n"
                << ";            Black: " << game_clock.time_left(BLACK) << "\n\n"
                << std::endl;
        }

        return end_game.winner();
    }
    catch(const std::exception& error)
    {
        board.ascii_draw(WHITE);
        board.print_game_record(white.name(), black.name(), pgn_file_name, error.what());
        throw;
    }
}
