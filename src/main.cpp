#include <iostream>
#include <memory>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <utility>

#include "Game/Game.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Players/Minimax_AI.h"
#include "Players/Random_AI.h"

#include "Genes/Gene_Pool.h"

#include "Utility/Exceptions.h"
#include "Utility/String.h"

#include "Testing.h"

//! \file

namespace
{
    //! \brief Print the command-line options for this program.
    void print_help();

    //! \brief Confirm that all moves in a PGN game record are legal moves.
    //!
    //! \param file_name The name of the file with the PGN game records. All games will be examined.
    bool confirm_game_record(const std::string& file_name);

    //! \brief Starts a single game according to command line options.
    //!
    //! \param argc The number of command line options (same as for main(int argc, char *argv[])).
    //! \param argv The command line options (same as for main(int argc, char *argv[])).
    void start_game(int argc, char* argv[]);

    //! \brief Reads a genome file and rewrites it in the latest style.
    //!
    //! \param file_name The name of the file to update.
    //! \exception Genetic_AI_Creation_Error or derivative if the genome file is invalid.
    void update_genome_file(const std::string& file_name);

    //! \brief Throws std::invalid_argument if assertion fails
    //!
    //! \param condition A condition that must be true to continue.
    //! \param failure_message A message to display if the assertion fails.
    void argument_assert(bool condition, const std::string& failure_message);
}

//! \brief The starting point for the whole program.
//!
//! \param argc The number of command-line arguments.
//! \param argv The command-line arguments. See print_help() (or run the program
//!        with no arguments) for a listing of all the options.
//! \returns EXIT_SUCCESS or EXIT_FAILURE.
int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
        print_help();
        return EXIT_SUCCESS;
    }

    try
    {
        const std::string option = argv[1];
        if(option == "-gene-pool")
        {
            argument_assert(argc > 2, "Specify a configuration file to run a gene pool.");
            gene_pool(argv[2]);
        }
        else if(option == "-confirm")
        {
            argument_assert(argc > 2, "Provide a file containing a game to confirm has all legal moves.");
            return confirm_game_record(argv[2]) ? EXIT_SUCCESS : EXIT_FAILURE;
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
        else if(option == "-update")
        {
            argument_assert(argc > 2, "Provide a file containing Genetic AI data.");
            update_genome_file(argv[2]);
        }
        else
        {
            start_game(argc, argv);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

namespace
{
    void print_help()
    {
        std::cout << "\n\nGenetic Chess\n"
                << "=============\n\n"
                << "Standalone functions (only first is run if multiple are specified):\n\n"
                << "\t-gene-pool [file name]\n"
                << "\t\tStart a run of a gene pool with parameters set in the given\n\t\tfile name.\n\n"
                << "\t-confirm [filename]\n"
                << "\t\tCheck a file containing PGN game records for any illegal moves\n\t\tor mismarked checks or checkmates.\n\n"
                << "\t-test\n"
                << "\t\tRun tests to ensure various parts of the program function\n\t\tcorrectly.\n\n"
                << "\t-speed\n"
                << "\t\tRun a speed test for gene scoring and boad move submission.\n\n"
                << "\t-perft\n"
                << "\t\tRun a legal move generation test.\n\n"
                << "\t-update [filename]\n"
                << "\t\tIf genetic_chess has changed how genomes are written, use\n\t\tthis option to update the file to the latest format.\n\n"
                << "The following options start a game with various players. If two players are\nspecified, the first plays white and the second black. If only one player is\nspecified, the program will wait for a CECP/xboard or UCI command from a GUI\nto start playing.\n\n"
                << "\t-genetic [filename [number]]\n"
                << "\t\tSelect a minimaxing genetic AI player for a game. Optional file name\n\t\tand ID number to load an AI from a file.\n\n"
                << "\t-random\n"
                << "\t\tSelect a player that makes random moves for a game.\n\n"
                << "Other game options:\n\n"
                << "\t-time [number]\n"
                << "\t\tSpecify the time (in seconds) each player has to play the game\n\t\tor to make a set number of moves (see -reset_moves option).\n\n"
                << "\t-reset-moves [number]\n"
                << "\t\tSpecify the number of moves a player must make within the time\n\t\tlimit. The clock adds the initial time every time this\n\t\tnumber of moves is made.\n\n"
                << "\t-increment-time [number]\n"
                << "\t\tSpecify seconds to add to time after each move.\n\n"
                << "\t-board [FEN string]\n"
                << "\t\tSpecify the starting board state using FEN notation. The entire\n\t\tstring should be quoted.\n\n"
                << "\t-event [name]\n"
                << "\t\tAn optional name for the game to be played. This name will be\n\t\twritten to the PGN game record.\n\n"
                << "\t-location [name]\n"
                << "\t\tAn optional location for the game to be played. This will be\n\t\twritten to the PGN game record.\n\n"
                << "\t-game-file [file name]\n"
                << "\t\tSpecify the name of the file where the game record should be\n\t\twritten. If none, record is printed to stdout.\n\n"
                << "All game options in this section can be overriden by GUI commands except\n-short-post, -event, -location, and -game-file.\n\n";
    }

    bool check_rule_result(const std::string& rule_source,
                           const std::string& rule_name,
                           const bool expected_ruling,
                           const bool actual_ruling,
                           const int last_move_line_number)
    {
        const auto pass = expected_ruling == actual_ruling;
        if( ! pass)
        {
            std::cerr << rule_source << " indicates "
                      << (expected_ruling ? "" : "no ")
                      << rule_name << ", but last move did "
                      << (actual_ruling ? "" : "not ")
                      << "trigger rule (line: " << last_move_line_number << ")." << std::endl;
        }

        return pass;
    }

    bool confirm_game_record(const std::string& file_name)
    {
        auto input = std::ifstream(file_name);
        if( ! input)
        {
            throw std::runtime_error("Could not open file " + file_name + " for reading.");
        }

        auto line_number = 0;
        auto last_move_line_number = 0;

        auto expected_winner = Winner_Color::NONE;
        auto expect_checkmate = true;
        auto expect_fifty_move_draw = false;
        auto expect_threefold_draw = false;
        auto in_game = false;
        Board board;
        Game_Result result;

        for(std::string line; std::getline(input, line);)
        {
            ++line_number;
            line = String::remove_pgn_comments(line);
            if(line.empty())
            {
                continue;
            }

            // Start header of new game
            if(in_game && line.starts_with("["))
            {
                if( ! check_rule_result("Header",
                                        "50-move draw",
                                        expect_fifty_move_draw,
                                        String::contains(result.ending_reason(), "50"),
                                        last_move_line_number))
                {
                    return false;
                }

                if( ! check_rule_result("Header",
                                        "threefold draw",
                                        expect_threefold_draw,
                                        String::contains(result.ending_reason(), "fold"),
                                        last_move_line_number))
                {
                    return false;
                }

                if( ! check_rule_result("Header",
                                        "checkmate",
                                        expect_checkmate,
                                        String::contains(result.ending_reason(), "mates"),
                                        last_move_line_number))
                {
                    return false;
                }

                expected_winner = Winner_Color::NONE;
                expect_checkmate = true;
                expect_fifty_move_draw = false;
                expect_threefold_draw = false;
                in_game = false;
                board = Board();
                result = {};
            }

            if(line.starts_with("[Result"))
            {
                if(String::contains(line, "1-0"))
                {
                    expected_winner = Winner_Color::WHITE;
                }
                else if(String::contains(line, "0-1"))
                {
                    expected_winner = Winner_Color::BLACK;
                }
                else if(String::contains(line, "1/2-1/2"))
                {
                    expect_checkmate = false;
                }
                else if(String::contains(line, '*'))
                {
                    expect_checkmate = false;
                }
                else
                {
                    std::cerr << "Malformed Result: " << line << " (line: " << line_number << ")" << std::endl;
                    return false;
                }
            }
            else if(line.starts_with("[Termination"))
            {
                expect_checkmate = false;
                if(String::contains(line, "fold"))
                {
                    expect_threefold_draw = true;
                }
                else if(String::contains(line, "50"))
                {
                    expect_fifty_move_draw = true;
                }
            }
            else if(line.starts_with("[FEN"))
            {
                board = Board(String::split(line, "\"").at(1));
            }
            else if(line.starts_with("["))
            {
                continue;
            }
            else // Line contains game moves
            {
                in_game = true;
                std::string move_number;
                for(const auto& move : String::split(line))
                {
                    if(move.back() == '.')
                    {
                        move_number = move + ' ';
                        continue;
                    }

                    if(board.whose_turn() == Piece_Color::BLACK)
                    {
                        move_number += "... ";
                    }

                    if((move == "1/2-1/2" && expected_winner != Winner_Color::NONE) ||
                       (move == "1-0" && expected_winner != Winner_Color::WHITE) ||
                       (move == "0-1" && expected_winner != Winner_Color::BLACK) ||
                       (move == "*" && expected_winner != Winner_Color::NONE))
                    {
                        std::cerr << "Final result mark (" << move << ") does not match game result. (line: " << line_number << ")" << std::endl;
                        return false;
                    }

                    if(move == "1/2-1/2" || move == "1-0" || move == "0-1" || move == "*")
                    {
                        continue;
                    }

                    try
                    {
                        const auto& move_to_play = board.interpret_move(move);
                        last_move_line_number = line_number;
                        if( ! check_rule_result("Move: " + move_number + move + ")",
                                                "capture",
                                                String::contains(move, 'x'),
                                                board.move_captures(move_to_play),
                                                last_move_line_number))
                        {
                            return false;
                        }

                        result = board.play_move(move_to_play);

                        if( ! check_rule_result("Move (" + move_number + move + ")",
                                                "check",
                                                String::contains("+#", move.back()),
                                                board.king_is_in_check(),
                                                last_move_line_number))
                        {
                            return false;
                        }

                        if( ! check_rule_result("Move (" + move_number + move + ")",
                                                "checkmate",
                                                move.back() == '#',
                                                result.game_has_ended() && result.winner() != Winner_Color::NONE,
                                                last_move_line_number))
                        {
                            return false;
                        }
                    }
                    catch(const Illegal_Move&)
                    {
                        std::cerr << "Move (" << move_number << move << ") is illegal."
                                  << " (line: " << line_number << ")\n";
                        std::cerr << "Legal moves: ";
                        for(const auto legal_move : board.legal_moves())
                        {
                            std::cerr << legal_move->algebraic(board) << " ";
                        }
                        std::cerr << '\n' << board.fen() << std::endl;
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void start_game(int argc, char* argv[])
    {
        // Use pointers since each player could be Genetic, Random, etc.
        std::unique_ptr<Player> white;
        std::unique_ptr<Player> black;
        std::unique_ptr<Player> latest;

        Clock::seconds game_time{};
        size_t moves_per_reset = 0;
        Clock::seconds increment_time{};
        Board board;
        std::string game_file_name;
        std::string event_name;
        std::string location;
        auto thinking_output = Thinking_Output_Type::NO_THINKING;

        for(int i = 1; i < argc; ++i)
        {
            const std::string opt = argv[i];
            if(opt == "-random")
            {
                latest = std::make_unique<Random_AI>();
            }
            else if(opt == "-genetic")
            {
                argument_assert(i + 1 < argc, "Genome file needed for Genetic AI player");
                std::string filename = argv[++i];

                try
                {
                    const auto id = i + 1 < argc ? argv[i + 1] : std::string{};
                    latest = std::make_unique<Minimax_AI>(filename, String::to_number<int>(id));
                    ++i;
                }
                catch(const std::invalid_argument&) // Could not convert id to an int.
                {
                    latest = std::make_unique<Minimax_AI>(filename, find_last_id(filename));
                }
            }
            else if(opt == "-time" && i + 1 < argc)
            {
                game_time = String::to_duration<Clock::seconds>(argv[++i]);
            }
            else if(opt == "-reset-moves" && i + 1 < argc)
            {
                moves_per_reset = String::to_number<size_t>(argv[++i]);
            }
            else if(opt == "-increment-time" && i + 1 < argc)
            {
                increment_time = String::to_duration<Clock::seconds>(argv[++i]);
            }
            else if(opt == "-board" && i + 1 < argc)
            {
                board = Board(argv[++i]);
            }
            else if(opt == "-game-file" && i + 1 < argc)
            {
                game_file_name = argv[++i];
            }
            else if(opt == "-event" && i + 1 < argc)
            {
                event_name = argv[++i];
            }
            else if(opt == "-location" && i + 1 < argc)
            {
                location = argv[++i];
            }
            else if(opt == "-xboard")
            {
                thinking_output = Thinking_Output_Type::CECP;
            }
            else if(opt == "-uci")
            {
                thinking_output = Thinking_Output_Type::UCI;
            }
            else
            {
                throw std::invalid_argument("Invalid or incomplete game option: " + opt);
            }

            if(latest)
            {
                if( ! white)
                {
                    white = std::move(latest);
                }
                else if( ! black)
                {
                    black = std::move(latest);
                }
                else
                {
                    throw std::invalid_argument("More than two players specified.");
                }
            }
        }

        if( ! white)
        {
            throw std::invalid_argument("At least one player must be specified.");
        }

        if( ! black)
        {
            play_game_with_outsider(*white, event_name, location, game_file_name);
        }
        else
        {
            Player::set_thinking_mode(thinking_output);
            play_game(board,
                      Clock(game_time, moves_per_reset, increment_time, Time_Reset_Method::ADDITION, board.whose_turn()),
                      *white, *black,
                      event_name,
                      location,
                      game_file_name);
        }
    }

    void update_genome_file(const std::string& file_name)
    {
        auto input = std::ifstream(file_name);
        std::vector<std::string> lines_to_write;
        auto skip_to_END = false;
        auto skip_next_blank_line = false;
        for(std::string line; std::getline(input, line);)
        {
            if(skip_to_END)
            {
                if(line == "END")
                {
                    skip_to_END = false;
                    skip_next_blank_line = true;
                }
                continue;
            }

            if(line.starts_with("ID:"))
            {
                lines_to_write.push_back(String::split(line, ":", 1).back());
                skip_to_END = true;
            }
            else
            {
                if( ! (skip_next_blank_line && line.empty()))
                {
                    lines_to_write.push_back(line);
                }
                skip_next_blank_line = false;
            }
        }

        input = std::ifstream(file_name);
        const auto output_file_name = String::add_to_file_name(file_name, "-updated");
        auto output = std::ofstream(output_file_name);
        std::cout << "Writing to: " << output_file_name << std::endl;
        for(const auto& line : lines_to_write)
        {
            try
            {
                Genetic_AI(input, std::stoi(line)).print(output);
            }
            catch(const Genetic_AI_Creation_Error& e)
            {
                std::cerr << e.what() << '\n';
            }
            catch(const std::invalid_argument&)
            {
                output << line << '\n';
            }
        }
    }

    void argument_assert(const bool condition, const std::string& failure_message)
    {
        if( ! condition)
        {
            throw std::invalid_argument(failure_message);
        }
    }
}
