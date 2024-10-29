#include "Puzzle.h"

#include <vector>
#include <string>
#include <tuple>
#include <stdexcept>
#include <fstream>
#include <format>
#include <iostream>
#include <chrono>
using namespace std::chrono_literals;

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Move.h"
#include "Players/Genetic_AI.h"
#include "Utility/Main_Tools.h"
#include "Utility/String.h"

//! \file
namespace Puzzle
{
    void solve(const std::vector<std::string>& fens_or_file_names, const std::vector<std::tuple<std::string, std::vector<std::string>>>& options)
    {
        auto puzzles_to_solve = std::vector<std::string>{};
        for(const auto& fen_or_file_name : fens_or_file_names)
        {
            try
            {
                const auto parse_board = Board(fen_or_file_name);
                puzzles_to_solve.push_back(parse_board.fen());
            }
            catch(const std::invalid_argument&)
            {
                auto file_input = std::ifstream(fen_or_file_name);
                if(!file_input)
                {
                    throw std::invalid_argument(std::format("The input is not an FEN or file name: {}", fen_or_file_name));
                }

                std::cout << std::format("Reading from file: {}", fen_or_file_name) << std::endl;
                std::string line;
                while(std::getline(file_input, line))
                {
                    const auto parse_board = Board{ line };
                    puzzles_to_solve.push_back(parse_board.fen());
                }
            }
        }

        Genetic_AI solver;
        for(const auto& [opt, values] : options)
        {
            if(opt == "-genetic")
            {
                Main_Tools::argument_assert(!values.empty(), "Genome file needed for player");
                std::string file_name = values[0];

                try
                {
                    const auto id = values.size() > 1 ? values[1] : std::string{};
                    solver = Genetic_AI(file_name, String::to_number<int>(id));
                }
                catch(const std::invalid_argument&) // Could not convert id to an int.
                {
                    solver = Genetic_AI(file_name, find_last_id(file_name));
                }
            }
        }

        for(const auto& puzzle : puzzles_to_solve)
        {
            solver.reset();
            std::cout << "\n=============\n\n" << puzzle << std::endl;
            const auto board = Board{ puzzle };
            board.cli_print(std::cout);
            const auto clock = Clock(120s, 1);
            solver.choose_move(board, clock);
            const auto result = solver.commentary_for_next_move(board);
            std::cout << "\n" << String::extract_delimited_text(result, '(', ')') << std::endl;
        }
    }
}
