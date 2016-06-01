#include "Game/Game.h"

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "Players/Player.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Moves/Move.h"
#include "Exceptions/Game_Ending_Exception.h"
#include "Exceptions/Out_Of_Time_Exception.h"
#include "Exceptions/Generic_Exception.h"

// Play single game, return color of winner
Color play_game(const Player& white,
                const Player& black,
                int time_in_seconds,
                int moves_to_reset,
                const std::string& pgn_file_name)
{
    const std::string long_file_name = "longest_game.pgn";
    static size_t longest_game = count_moves(long_file_name);
    size_t move_count = 0;
    Color winner;
    std::string result;

    Board board;
    Clock game_clock(time_in_seconds, moves_to_reset);
    game_clock.start();

    try
    {
        while(true)
        {
            move_count += (board.whose_turn() == WHITE ? 1 : 0);
			auto& player = board.whose_turn() == WHITE ? white : black;
            auto move_chosen = player.choose_move(board, game_clock);
            game_clock.punch();
            board.submit_move(move_chosen);
        }
    }
    catch(const Game_Ending_Exception& end_game)
    {
        result = end_game.what();
        winner = end_game.winner();

		// for Outside_Players communicating with xboard and the like
        white.process_game_ending(end_game);
        black.process_game_ending(end_game);
    }
    catch(const std::exception& error)
    {
        result = error.what();
        board.ascii_draw(WHITE);
        board.print_game_record(white.name(), black.name(), pgn_file_name, result);
        throw;
    }

    board.ascii_draw();
    board.print_game_record(white.name(), black.name(), "", result);
    if(move_count > longest_game)
    {
        board.print_game_record(white.name(), black.name(), long_file_name, result);
        longest_game = move_count;
    }

    if( ! pgn_file_name.empty())
    {
        board.print_game_record(white.name(), black.name(), pgn_file_name, result);
    }

    if(game_clock.is_running())
    {
        std::cout << "Time left: White: " << game_clock.time_left(WHITE)
                << "\n           Black: " << game_clock.time_left(BLACK) << std::endl;
        std::cout << result << std::endl;
    }

    return winner;
}

size_t count_moves(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    std::string line;
    size_t count = 0;
    while(std::getline(ifs, line))
    {
        if(isdigit(line.front()))
        {
            ++count;
        }
    }

    return count;
}
