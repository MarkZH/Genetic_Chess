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
    //! Print the command-line options for this program.
    void print_help();

    //! Confirm that all moves in a PGN game record are legal moves.
    //
    //! \param file_name The name of the file with the PGN game records. All games will be examined.
    bool confirm_game_record(const std::string& file_name);

    //! Find the last ID of a Genetic_AI in a gene pool file.
    //
    //! \param file_name The name of the file with Genetic_AI data.
    //! \returns The numerical ID of the last AI in the file.
    int find_last_id(const std::string& file_name);
}

//! The starting point for the whole program.
//
//! \param argc The number of command-line arguments.
//! \param argv The command-line arguments. See print_help() (or run the program
//!        with no arguments) for a listing of all the options.
//! \returns Zero for normal exit, non-zero for error exits.
int main(int argc, char *argv[])
{
    try
    {
        if(argc > 1)
        {
            std::string option = argv[1];
            if(option == "-genepool")
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
                        return 2;
                    }
                }
                else
                {
                    std::cerr << "Provide a file containing a game to confirm has all legal moves." << std::endl;
                    return 1;
                }
            }
            else if(option == "-test")
            {
                return run_tests() ? 0 : 1;
            }
            else if(option == "-speed")
            {
                run_speed_tests();
            }
            else if(option == "-perft")
            {
                return run_perft_tests() ? 0 : 1;
            }
            else if(option == "-random_test")
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
                        int id = -1;
                        if(i + 1 < argc)
                        {
                            filename = argv[i+1];
                            if(filename.front() == '-')
                            {
                                filename.clear();
                            }
                        }

                        if(i + 2 < argc)
                        {
                            try
                            {
                                id = std::stoi(argv[i+2]);
                            }
                            catch(const std::exception&)
                            {
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
                            if(id < 0)
                            {
                                latest = std::make_unique<Genetic_AI>(filename, find_last_id(filename));
                                i += 1;
                            }
                            else
                            {
                                latest = std::make_unique<Genetic_AI>(filename, id);
                                i += 2;
                            }
                        }
                    }
                    else if(opt == "-time" && i + 1 < argc)
                    {
                        game_time = std::stod(argv[++i]);
                    }
                    else if(opt == "-reset_moves" && i + 1 < argc)
                    {
                        moves_per_reset = String::string_to_number<size_t>(argv[++i]);
                    }
                    else if(opt == "-increment_time" && i + 1 < argc)
                    {
                        increment_time = std::stod(argv[++i]);
                    }
                    else if(opt == "-board" && i + 1 < argc)
                    {
                        board = Board(argv[++i]);
                    }
                    else if(opt == "-game_file" && i + 1 < argc)
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
                    else if(opt == "-short_post")
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
                              Clock(game_time, moves_per_reset, increment_time, board.whose_turn()),
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
        return 1;
    }

    return 0;
}

namespace
{
    void print_help()
    {
        std::cout << "\n\nGenetic Chess\n"
                << "=============\n\n"
                << "Standalone functions (only first is run if multiple are specified):\n\n"
                << "\t-genepool [file name]\n"
                << "\t\tStart a run of a gene pool with parameters set in the given\n\t\tfile name.\n\n"
                << "\t-confirm [filename]\n"
                << "\t\tCheck a file containing PGN game records for any illegal moves\n\t\tor mismarked checks or checkmates.\n\n"
                << "\t-test\n"
                << "\t\tRun tests to ensure various parts of the program function\n\t\tcorrectly.\n\n"
                << "\t-speed\n"
                << "\t\tRun a speed test for gene scoring and boad move submission.\n\n"
                << "\t-perft\n"
                << "\t\tRun a legal move generation test.\n\n"
                << "\t-random_test\n"
                << "\t\tGenerate a sample of random numbers for a quick check of quality.\n\n"
                << "The following options start a game with various players. If two players are\nspecified, the first plays white and the second black. If only one player is\nspecified, the program will wait for a CECP command from outside to start\nplaying.\n\n"
                << "\t-genetic [filename [number]]\n"
                << "\t\tSelect a genetic AI player for a game. Optional file name and\n\t\tID number to load an AI from a file.\n\n"
                << "\t-random\n"
                << "\t\tSelect a player that makes random moves for a game.\n\n"
                << "Other game options:\n\n"
                << "\t-time [number]\n"
                << "\t\tSpecify the time (in seconds) each player has to play the game\n\t\tor to make a set number of moves (see -reset_moves option).\n\n"
                << "\t-reset_moves [number]\n"
                << "\t\tSpecify the number of moves a player must make within the time\n\t\tlimit. The clock adds the initial time every time this\n\t\tnumber of moves is made.\n\n"
                << "\t-increment_time [number]\n"
                << "\t\tSpecify seconds to add to time after each move.\n\n"
                << "\t-board [FEN string]\n"
                << "\t\tSpecify the starting board state using FEN notation. The entire\n\t\tstring should be quoted.\n\n"
                << "\t-pondering\n"
                << "\t\tAllow AI players to think ahead when it is not their turn.\n\n"
                << "\t-game_file [file name]\n"
                << "\t\tSpecify the name of the file where the game record should be\n\t\twritten. If none, record is printed to stdout.\n\n";
    }

    int find_last_id(const std::string& players_file_name)
    {
        std::ifstream player_input(players_file_name);
        std::string line;
        int last_player = -1;
        while(std::getline(player_input, line))
        {
            if(String::starts_with(line, "ID:"))
            {
                last_player = std::stoi(String::split(line).back());
            }
        }

        return last_player;
    }

    bool confirm_game_record(const std::string& file_name)
    {
        auto input = std::ifstream(file_name);
        std::string line;
        auto line_number = 0;
        auto last_move_line_number = 0;

        auto expected_winner = NONE;
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
            line = String::strip_block_comment(line, "(", ")");
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

                expected_winner = NONE;
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
                    expected_winner = WHITE;
                }
                else if(String::contains(line, "0-1"))
                {
                    expected_winner = BLACK;
                }
                else if(String::contains(line, "1/2-1/2"))
                {
                    expected_winner = NONE;
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

                    if(board.whose_turn() == BLACK)
                    {
                        move_number += "... ";
                    }

                    if((move == "1/2-1/2" && expected_winner != NONE) ||
                    (move == "1-0" && expected_winner != WHITE) ||
                    (move == "0-1" && expected_winner != BLACK))
                    {
                        std::cerr << "Final result mark (" << move << ") does not match game result. (line: " << line_number << ")" << std::endl;
                        return false;
                    }

                    if(move == "1/2-1/2" || move == "1-0" || move == "0-1")
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
                            const Board& temp = board; // to prevent use of non-const private overload
                            if( ! temp.piece_on_square(move_to_submit.end()) && ! move_to_submit.is_en_passant())
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
                            if(result.winner() != opposite(board.whose_turn()))
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
                            if(result.winner() != NONE)
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
                        return false;
                    }
                }
            }
        }

        return true;
    }
}
