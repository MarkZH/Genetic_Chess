#ifndef PUZZLE_H
#define PUZZLE_H

#include <vector>
#include <string>
#include <tuple>

namespace Puzzle
{
    //! Solve a chess puzzle.
    //! 
    //! \param fens_or_file_names The argument to the -solve option should be a list of FENs, a list of file names with one FEN per line, or both.
    //! \param options The rest of the command line so that a player may be chosen to solve the puzzles.
    void solve(const std::vector<std::string>& fens_or_file_names, const std::vector<std::tuple<std::string, std::vector<std::string>>>& options);
}

#endif // !PUZZLE_H
