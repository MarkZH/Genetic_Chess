#include <iostream>
#include <memory>
#include <fstream>

#include "Game/Game.h"

#include "Players/Genetic_AI.h"
#include "Players/Human_Player.h"
#include "Players/Random_AI.h"
#include "Players/Outside_Player.h"

#include "Genes/Gene_Pool.h"

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
            else
            {
                std::unique_ptr<Player> white;
                std::unique_ptr<Player> black;
                std::unique_ptr<Player> latest;

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
                    else
                    {
                        throw std::runtime_error("Invalid player option: " + opt);
                    }

                    if(white)
                    {
                        black = std::move(latest);
                        break;
                    }
                    else
                    {
                        white = std::move(latest);
                    }
                }

                if( ! white || ! black)
                {
                    std::cerr << "Choose two players.\n";
                    return 1;
                }
                play_game(*white, *black, 0, 0, "game.pgn");
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
