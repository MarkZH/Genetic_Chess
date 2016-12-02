#include <iostream>
#include <memory>
#include <fstream>

#include "Game/Game.h"
#include "Game/Board.h"

#include "Moves/Move.h"

#include "Players/Genetic_AI.h"
#include "Players/Human_Player.h"
#include "Players/Random_AI.h"
#include "Players/Outside_Player.h"

#include "Genes/Gene_Pool.h"

#include "Exceptions/Illegal_Move_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"

#include "Utility.h"
#include "Testing.h"

int main(int argc, char *argv[])
{
    try
    {
        run_tests();

        if(argc > 1)
        {
            if(std::string(argv[1]) == "-genepool")
            {
                std::string gene_pool_config_file_name;
                if(argc > 2)
                {
                    gene_pool_config_file_name = argv[2];
                }

                gene_pool(gene_pool_config_file_name);
            }
            else if(std::string(argv[1]) == "-replay")
            {
                if(argc > 2)
                {
                    Board board;
                    std::string file_name = argv[2];
                    std::ifstream ifs(file_name);
                    std::string line;
                    bool game_started = false;
                    while( ! board.game_has_ended() && std::getline(ifs, line))
                    {
                        line = String::strip_comments(line, ';');
                        line = String::strip_comments(line, '{');
                        line = String::strip_comments(line, '[');
                        if(line.empty())
                        {
                            if(game_started)
                            {
                                break;
                            }
                            else
                            {
                                continue;
                            }
                        }

                        for(const auto& s : String::split(line))
                        {
                            try
                            {
                                board.submit_move(board.get_complete_move(s));
                            }
                            catch(const Illegal_Move_Exception&)
                            {
                                std::cout << "Ignoring: " << s << std::endl;
                                continue;
                            }
                            catch(const Game_Ending_Exception& e)
                            {
                                std::cout << e.what() << std::endl;
                            }
                            board.ascii_draw(WHITE);
                            game_started = true;
                            std::cout << "Last move: ";
                            std::cout << (board.get_game_record().size() + 1)/2 << ". ";
                            std::cout << (board.whose_turn() == WHITE ? "... " : "");
                            std::cout << board.get_game_record().back() << std::endl;
                            if(board.game_has_ended())
                            {
                                break;
                            }

                            std::cout << "Enter \"y\" to play game from here: " << std::endl;
                            char response = std::cin.get();
                            if(std::tolower(response) == 'y')
                            {
                                play_game_with_board(Human_Player(),
                                                     Human_Player(),
                                                     0,
                                                     0,
                                                     file_name + "_continued.pgn",
                                                     board);
                                 break;
                            }
                        }
                    }
                }
                else
                {
                    std::cerr << "-replay must be followed by a file name." << std::endl;
                    return 1;
                }
            }
            else
            {
                std::unique_ptr<Player> white;
                std::unique_ptr<Player> black;
                std::unique_ptr<Player> latest;

                int game_time = 0;
                int moves_per_reset = 0;

                for(int i = 1; i < argc; ++i)
                {
                    std::string opt = argv[i];
                    if(opt == "-human")
                    {
                        latest.reset(new Human_Player());
                    }
                    else if(opt == "-random")
                    {
                        latest.reset(new Random_AI());
                    }
                    else if(opt == "-genetic")
                    {
                        Genetic_AI *genetic_ptr = nullptr;
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
                            genetic_ptr = new Genetic_AI;
                            for(int j = 0; j < 100; ++j)
                            {
                                genetic_ptr->mutate();
                            }
                        }
                        else
                        {
                            if(id < 0)
                            {
                                genetic_ptr = new Genetic_AI(filename);
                                i += 1;
                            }
                            else
                            {
                                genetic_ptr = new Genetic_AI(filename, id);
                                i += 2;
                            }
                        }

                        latest.reset(genetic_ptr);
                    }
                    else if(opt == "-time")
                    {
                        game_time = std::stoi(argv[++i]);
                    }
                    else if(opt == "-reset_moves")
                    {
                        moves_per_reset = std::stoi(argv[++i]);
                    }
                    else
                    {
                        throw std::runtime_error("Invalid player option: " + opt);
                    }

                    if(latest)
                    {
                        if( ! white)
                        {
                            white = std::move(latest);
                            continue;
                        }
                        if( ! black)
                        {
                            black = std::move(latest);
                            continue;
                        }
                    }
                }

                if( ! white || ! black)
                {
                    std::cerr << "Choose two players.\n";
                    return 1;
                }
                play_game(*white, *black, game_time, moves_per_reset, "game.pgn");
            }
        }
        else
        {
            auto outside = Outside_Player();
            auto genetic = Genetic_AI();

            if(outside.get_ai_color() == WHITE)
            {
                play_game(genetic, outside, 0, 0, "");
            }
            else
            {
                play_game(outside, genetic, 0, 0, "");
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
