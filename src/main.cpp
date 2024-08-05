#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "Game/Game.h"
#include "Game/PGN.h"

#include "Genes/Gene_Pool.h"

#include "Utility/String.h"
#include "Utility/Main_Tools.h"

#include "Testing.h"

//! \file

//! \brief The starting point for the whole program.
//!
//! \param argc The number of command-line arguments.
//! \param argv The command-line arguments. See print_help() (or run the program
//!        with no arguments or with -help) for a listing of all the options.
//! \returns EXIT_SUCCESS or EXIT_FAILURE.
int main(int argc, char *argv[])
{
    try
    {
        const auto options = Main_Tools::parse_options(argc, argv);
        const auto& [option, parameters] = options.front();
        if(option == "-gene-pool")
        {
            Main_Tools::argument_assert( ! parameters.empty(), "Specify a configuration file to run a gene pool.");
            gene_pool(parameters[0]);
        }
        else if(option == "-confirm")
        {
            Main_Tools::argument_assert( ! parameters.empty(), "Provide a file containing games to confirm they have all legal moves.");
            return PGN::confirm_game_record(parameters[0]) ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else if(option == "-test")
        {
            return run_tests() ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else if(option == "-speed")
        {
            run_speed_tests();
        }
        else if(option == "-perft")
        {
            return run_perft_tests() ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else if(option == "-list")
        {
            Main_Tools::argument_assert( ! parameters.empty(), option + " requires a numeric argument.");
            list_moves(String::to_number<size_t>(parameters[0]));
        }
        else if(option == "-help")
        {
            Main_Tools::print_help();
        }
        else
        {
            start_game(options);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\nERROR: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
