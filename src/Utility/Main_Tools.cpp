#include "Utility/Main_Tools.h"

#include "Utility/String.h"
#include "Utility/Help_Writer.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

void Main_Tools::print_help()
{
    Help_Writer help;
    help.add_title("Genetic Chess");
    help.add_paragraph("Genetic Chess is a program that runs a chess engine to play games or run a gene pool for genetically optimizing the chess engine.");
    help.add_section_title("Command-line Arguments");
    help.add_paragraph("For arguments with parameters, all of the following forms are equivalent:");
    help.add_option("-arg param");
    help.add_option("-arg=param");
    help.add_option("--arg param");
    help.add_option("--arg=param");
    help.add_paragraph("where arg is the name of the argument and param is a single required parameter.");
    help.add_paragraph("Parameters in [square~brackets] are required, while parameters in <angle~brackets> are optional.");
    help.add_paragraph("For arguments without parameters, the following forms are equivalent:");
    help.add_option("-arg");
    help.add_option("--arg");
    help.add_section_title("Standalone functions");
    help.add_paragraph("These functions are for single program actions. If multiple of these options are specified, only first is run before exiting.");
    help.add_option("-help", "Print this help text and exit.");
    help.add_option("-gene-pool", {"file name"}, "Start a run of a gene pool with parameters from the given file.");
    help.add_option("-confirm", {"file name"}, "Check a file containing PGN game records for any illegal moves or mismarked checks or checkmates.");
    help.add_option("-test", "Run tests to ensure various parts of the program function correctly.");
    help.add_option("-speed", "Run a speed test for gene scoring and board move submission.");
    help.add_option("-perft", "Run a legal move generation speed test.");
    help.add_section_title("Player options");
    help.add_paragraph("The following options start a game with various players. If two players are specified, the first plays white and the second black. If only one player is specified, the program will wait for an Xboard or UCI command from a GUI to start playing.");
    help.add_option("-genetic", {"file name"}, {"ID number"}, "Select a minimaxing evolved player for a game and load data from the file. If there are multiple genomes in the file, specify an ID number to load, otherwise the last genome in the file will be used.");
    help.add_option("-random", "Select a player that makes random moves for a game.");
    help.add_section_title("Other game options");
    help.add_option("-time", {"number"}, "Specify the time (in seconds) each player has to play the game or to make a set number of moves (see -reset_moves option).");
    help.add_option("-reset-moves", {"number"}, "Specify the number of moves a player must make within the time limit. The clock adds the initial time every time this number of moves is made.");
    help.add_option("-increment-time", {"number"}, "Specify seconds to add to time after each move.");
    help.add_option("-board", {"FEN string"}, "Specify the starting board state using FEN notation. The entire string should be quoted.");
    help.add_option("-event", {"name"}, "An optional name for the game to be played. This name will be written to the PGN game record in the Event tag.");
    help.add_option("-location", {"name"}, "An optional location for the game to be played. This will be written to the PGN game record in the Location tag.");
    help.add_option("-game-file", {"file name"}, "Specify the name of the file where the game record should be written. If none, record is printed to stdout.");
    help.add_option("-uci");
    help.add_option("-xboard", "Show an engine's thinking output in either UCI or Xboard format.");
    help.add_option("-show-board", "Show the board on the command line when playing a local game.");
    help.add_option("-log-comms", "Log UCI/Xboard communications (except engine thinking) to a file.");
    help.add_paragraph("All game options in this section can be overriden by GUI commands except -event, -location, and -game-file.");

    std::cout << help;
}

void Main_Tools::argument_assert(const bool condition, const std::string& failure_message)
{
    if( ! condition)
    {
        throw std::invalid_argument(failure_message);
    }
}

std::vector<std::string> Main_Tools::standardize_options(int argc, char* argv[])
{
    std::vector<std::string> options;
    for(int i = 1; i < argc; ++i)
    {
        if(String::starts_with(argv[i], "-"))
        {
            auto parts = String::split(argv[i], "=", 1);
            if(String::starts_with(parts.front(), "--"))
            {
                parts.front() = parts.front().substr(1);
            }
            options.insert(options.end(), parts.begin(), parts.end());
        }
        else
        {
            options.push_back(argv[i]);
        }
    }

    if(options.empty())
    {
        options.push_back("-help");
    }

    return options;
}
