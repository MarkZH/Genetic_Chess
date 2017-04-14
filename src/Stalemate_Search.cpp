#include "Stalemate_Search.h"

// https://www.chess.com/article/view/the-shortest-stalemate-possible
// https://en.wikipedia.org/wiki/Stalemate#Stalemate_in_problems
// https://en.wikipedia.org/wiki/Sam_Loyd

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "Game/Board.h"
#include "Moves/Complete_Move.h"
#include "Exceptions/Game_Ending_Exception.h"

std::vector<std::string> search_stalemate(const Board& board, int max_depth);

void stalemate_search_start()
{
    for(int max_depth = 0; max_depth < 22; ++max_depth) // 22-ply stalemate found by Sam Loyd
    {
        auto time_start = std::chrono::steady_clock::now();
        Board board;
        auto result = search_stalemate(board, max_depth);
        auto time_end = std::chrono::steady_clock::now();
        std::cout << "Time taken = "
                  << std::chrono::duration_cast<std::chrono::duration<double>>(time_end - time_start).count()
                  << " seconds." << std::endl;
        if( ! result.empty())
        {
            Board b;
            for(const auto& item : result)
            {
                auto move = b.get_complete_move(item);
                std::cout << move.game_record_item(b) << " ";
                b.submit_move(move);
            }
            std::cout << std::endl;
            break;
        }
        else
        {
            std::cout << "No stalemate after " << max_depth << " moves." << std::endl;
        }
    }
}

std::vector<std::string> search_stalemate(const Board& board, const int max_depth)
{
    if(max_depth <= 0)
    {
        return {}; // too deep
    }

    for(const auto& move : board.all_legal_moves())
    {
        auto next_board = board;
        try
        {
            next_board.submit_move(move);
            auto result = search_stalemate(next_board, max_depth - 1);
            if( ! result.empty())
            {
                return result; // pass up result from below
            }
        }
        catch(const Game_Ending_Exception& gee)
        {
            if(gee.what() == std::string("Stalemate"))
            {
                return next_board.get_game_record(); // found stalemate
            }
        }
    }

    return {}; // no stalemate at most max_depth from here
}
