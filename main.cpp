#include <iostream>
#include <memory>
#include <fstream>

#include "Game/Game.h"
#include "Players/Genetic_AI.h"
#include "Players/Human_Player.h"
#include "Players/Random_AI.h"
#include "Players/Outside_Player.h"

#include "Genes/Gene_Pool.h"

#include "Exceptions/Generic_Exception.h"

#ifdef DEBUG
    #include "Testing.h"
#endif // DEBUG

int main(int argc, char *argv[])
{
    try
    {
        #ifdef DEBUG
            run_tests();
        #endif // DEBUG

        if(argc > 1)
        {
            if(std::string(argv[1]) == "-genepool")
            {
                std::string gene_pool_file_name;
                if(argc > 2)
                {
                    gene_pool_file_name = argv[2];
                }

                gene_pool(gene_pool_file_name);
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
                        if(i + 1 < argc)
                        {
                            try
                            {
                                latest.reset(new Genetic_AI(argv[i+1]));
                                ++i;
                            }
                            catch(const Generic_Exception&)
                            {
                                std::cout << argv[i+1] << " not a file. Treating as next argument."
                                          << std::endl;
                                latest.reset(new Genetic_AI);
                            }
                        }
                        else
                        {
                            latest.reset(new Genetic_AI);
                        }
                    }
                    else
                    {
                        throw Generic_Exception("Invalid player option: " + opt);
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
        std::ofstream("error_log.txt", std::ios::app) << "\n\nERROR: " << e.what() << std::endl;
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
