#include <iostream>
#include <memory>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <string>

#include "Game/Game.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Players/Genetic_AI.h"
#include "Players/Random_AI.h"

#include "Genes/Gene_Pool.h"

#include "Exceptions/Illegal_Move.h"

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
}

//! \brief The starting point for the whole program.
//!
//! \param argc The number of command-line arguments.
//! \param argv The command-line arguments. See print_help() (or run the program
//!        with no arguments) for a listing of all the options.
//! \returns EXIT_SUCCESS or EXIT_FAILURE.
int main(int argc, char *argv[])
{
    try
    {
        if(argc > 1)
        {
            std::string option = argv[1];
            if(option == "-gene-pool")
            {
                std::string gene_pool_config_file_name;
                if(argc > 2)
                {
                    gene_pool_config_file_name = argv[2];
                }

                if(gene_pool_config_file_name.empty())
                {
                    throw std::invalid_argument("Specify a configuration file to run a gene pool.");
                }

                gene_pool(gene_pool_config_file_name);
            }
            else if(option == "-confirm")
            {
                if(argc >= 3)
                {
                    if( ! confirm_game_record(argv[2]))
                    {
                        std::cerr << "Game contains illegal or mismarked moves." << std::endl;
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    std::cerr << "Provide a file containing a game to confirm has all legal moves." << std::endl;
                    return EXIT_FAILURE;
                }
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
            else if(option == "-random-test")
            {
                print_randomness_sample();
            }
            else
            {
                // Use pointers since each player could be Genetic, Random, etc.
                std::unique_ptr<Player> white;
                std::unique_ptr<Player> black;
                std::unique_ptr<Player> latest;

                double game_time = 0;
                size_t moves_per_reset = 0;
                double increment_time = 0;
                Board board;
                bool pondering_allowed = false;
                std::string game_file_name;
                std::string event_name;
                std::string location;

                for(int i = 1; i < argc; ++i)
                {
                    std::string opt = argv[i];
                    if(opt == "-random")
                    {
                        latest = std::make_unique<Random_AI>();
                    }
                    else if(opt == "-genetic")
                    {
                        std::string filename;
                        if(i + 1 < argc)
                        {
                            filename = argv[i+1];
                            if(filename.front() == '-')
                            {
                                filename.clear();
                            }
                        }

                        if(filename.empty())
                        {
                            auto genetic_player = std::make_unique<Genetic_AI>();
                            genetic_player->mutate(10000);
                            genetic_player->print("single_game_player.txt");
                            latest = std::move(genetic_player);
                        }
                        else
                        {
                            if(i + 2 < argc)
                            {
                                try
                                {
                                    latest = std::make_unique<Genetic_AI>(filename, std::stoi(argv[i + 2]));
                                    i += 2;
                                }
                                catch(const std::exception&)
                                {
                                }
                            }

                            if( ! latest)
                            {
                                latest = std::make_unique<Genetic_AI>(filename, find_last_id(filename));
                                i += 1;
                            }
                        }
                    }
                    else if(opt == "-time" && i + 1 < argc)
                    {
                        game_time = std::stod(argv[++i]);
                    }
                    else if(opt == "-reset-moves" && i + 1 < argc)
                    {
                        moves_per_reset = String::string_to_number<size_t>(argv[++i]);
                    }
                    else if(opt == "-increment-time" && i + 1 < argc)
                    {
                        increment_time = std::stod(argv[++i]);
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
                    else if(opt == "-pondering")
                    {
                        pondering_allowed = true;
                    }
                    else if(opt == "-short-post")
                    {
                        Player::set_short_post();
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
                    play_game(board,
                              Clock(game_time, moves_per_reset, increment_time, Time_Reset_Method::ADDITION, board.whose_turn()),
                              *white, *black,
                              pondering_allowed,
                              event_name,
                              location,
                              game_file_name);
                }
            }
        }
        else
        {
            print_help();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
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
                << "\t-random-test\n"
                << "\t\tGenerate a sample of random numbers for a quick check of quality.\n\n"
                << "The following options start a game with various players. If two players are\nspecified, the first plays white and the second black. If only one player is\nspecified, the program will wait for a CECP/xboard or UCI command from a GUI\nto start playing.\n\n"
                << "\t-genetic [filename [number]]\n"
                << "\t\tSelect a genetic AI player for a game. Optional file name and\n\t\tID number to load an AI from a file.\n\n"
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
                << "\t-pondering\n"
                << "\t\tAllow AI players to think ahead when it is not their turn.\n\n"
                << "\t-short-post\n"
                << "\t\tUse a shortened form of CECP/xboard thinking output. Some GUIs\n\t\tcannot handle the longer form.\n\n"
                << "\t-event [name]\n"
                << "\t\tAn optional name for the game to be played. This name will be\n\t\twritten to the PGN game record.\n\n"
                << "\t-location [name]\n"
                << "\t\tAn optional location for the game to be played. This will be\n\t\twritten to the PGN game record.\n\n"
                << "\t-game-file [file name]\n"
                << "\t\tSpecify the name of the file where the game record should be\n\t\twritten. If none, record is printed to stdout.\n\n"
                << "All game options in this section can be overriden by GUI commands except -short-post,\n-event, -location, and -game-file.\n\n";
    }

    bool confirm_game_record(const std::string& file_name)
    {
        auto input = std::ifstream(file_name);
        if( ! input)
        {
            throw std::runtime_error("Could not open file " + file_name + " for reading.");
        }

        std::string line;
        auto line_number = 0;
        auto last_move_line_number = 0;

        auto expected_winner = Winner_Color::NONE;
        auto expect_checkmate = true;
        auto expect_fifty_move_draw = false;
        auto expect_threefold_draw = false;
        auto in_game = false;
        Board board;
        Game_Result result;

        while(std::getline(input, line))
        {
            ++line_number;
            line = String::strip_block_comment(line, "{", "}");
            line = String::strip_nested_block_comments(line, "(", ")");
            line = String::strip_comments(line, ";");
            line = String::remove_extra_whitespace(line);
            if(line.empty())
            {
                continue;
            }

            // Start header of new game
            if(in_game && String::starts_with(line, "["))
            {
                if(expect_fifty_move_draw != String::contains(result.ending_reason(), "50"))
                {
                    std::cerr << "Header indicates 50-move draw, but last move did not trigger rule (line: " << last_move_line_number << ")." << std::endl;
                    return false;
                }

                if(expect_threefold_draw != String::contains(result.ending_reason(), "fold"))
                {
                    std::cerr << "Header indicates threefold draw, but last move did not trigger rule (line: " << last_move_line_number << ")." << std::endl;
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

            if(String::starts_with(line, "[Result"))
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
            else if(String::starts_with(line, "[Termination"))
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
            else if(String::starts_with(line, "[FEN"))
            {
                board = Board(String::split(line, "\"").at(1));
            }
            else if(String::starts_with(line, "["))
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
                        auto move_checkmates = move.back() == '#';
                        auto move_checks = move_checkmates || move.back() == '+';
                        auto& move_to_submit = board.create_move(move);
                        last_move_line_number = line_number;
                        if(String::contains(move, 'x')) // check that move captures
                        {
                            if( ! const_cast<const Board&>(board).piece_on_square(move_to_submit.end()) && ! move_to_submit.is_en_passant())
                            {
                                std::cerr << "Move: " << move_number << move << " indicates capture but does not capture. (line: " << line_number << ")" << std::endl;
                                return false;
                            }
                        }

                        result = board.submit_move(move_to_submit);

                        if(move_checks)
                        {
                            if( ! board.king_is_in_check())
                            {
                                std::cerr << "Move (" << move_number << move << ") indicates check but does not check. (line: " << line_number << ")" << std::endl;
                                return false;
                            }
                        }
                        else
                        {
                            if(board.king_is_in_check())
                            {
                                std::cerr << "Move (" << move_number << move << ") indicates no check but does check. (line: " << line_number << ")" << std::endl;
                                return false;
                            }
                        }

                        if(move_checkmates)
                        {
                            if(result.winner() != static_cast<Winner_Color>(opposite(board.whose_turn())))
                            {
                                std::cerr << "Move (" << move_number << move << ") indicates checkmate, but move does not checkmate. (line: " << line_number << ")" << std::endl;
                                return false;
                            }

                            if( ! expect_checkmate)
                            {
                                std::cerr << "Game ends in checkmate, but this is not indicated in headers. (line: " << line_number << ")" << std::endl;
                                return false;
                            }
                        }
                        else
                        {
                            if(result.winner() != Winner_Color::NONE)
                            {
                                std::cerr << "Move (" << move_number << move << ") does not indicate checkmate, but move does checkmate. (line: " << line_number << ")" << std::endl;
                                return false;
                            }
                        }
                    }
                    catch(const Illegal_Move& error)
                    {
                        std::cerr << "Move (" << move_number << move << ") is illegal: "
                                  << error.what()
                                  << ". (line: " << line_number << ")" << std::endl;
                        std::cerr << "Legal moves: ";
                        for(auto legal_move : board.legal_moves())
                        {
                            std::cerr << legal_move->algebraic(board) << " ";
                        }
                        std::cerr << std::endl;
                        std::cerr << board.fen() << std::endl;
                        return false;
                    }
                }
            }
        }

        return true;
    }
}
