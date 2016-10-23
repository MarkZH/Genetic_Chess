#include "Testing.h"

#ifdef DEBUG
#include <memory>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Game/Board.h"
#include "Moves/Move.h"
#include "Players/Genetic_AI.h"

#include "Utility.h"

#include "Exceptions/Illegal_Move_Exception.h"

// Declaration to silence warnings
bool files_are_identical(const std::string& file_name1, const std::string& file_name2);

bool files_are_identical(const std::string& file_name1, const std::string& file_name2)
{
    std::ifstream file1(file_name1);
    std::ifstream file2(file_name2);
    int line_count = 0;

    while(true)
    {
        std::string line1, line2;
        std::getline(file1, line1);
        std::getline(file2, line2);
        ++line_count;

        if(line1 != line2)
        {
            std::cerr << "Mismatch at line " << line_count << ":\n";
            std::cerr << line1 << " != " << line2 << "\n";
            return false;
        }

        if( ! file1 && ! file2)
        {
            break;
        }
    }

    return true;
}

void run_tests()
{
    bool tests_passed = true;

    // Castling with attacking piece
    for(auto castle_side : std::string("KQ"))
    {
        for(int rook_left_space = 0; rook_left_space < 8; ++rook_left_space)
        {
            auto rook_left  = (rook_left_space > 0 ? std::to_string(rook_left_space) : std::string());
            auto rook_right = (rook_left_space < 7 ? std::to_string(7 - rook_left_space) : std::string());
            Board board("1k6/" + rook_left + "r" + rook_right + "/8/8/8/8/8/R3K2R w KQ - 0 1");

            char final_file = (castle_side == 'K' ? 'g' : 'c');

            try
            {
                board.get_complete_move('e', 1, final_file, 1);
            }
            catch(const Illegal_Move_Exception&)
            {
                if(castle_side == 'K')
                {
                    if(rook_left_space >= 4 && rook_left_space <= 6)
                    {
                        continue;
                    }
                }
                else
                {
                    if(rook_left_space >= 2 && rook_left_space <= 4)
                    {
                        continue;
                    }
                }

                board.ascii_draw(WHITE);
                std::cout << std::string(1, castle_side) + "-castle should be legal here." << std::endl;
                tests_passed = false;
            }

            if(castle_side == 'K')
            {
                if(rook_left_space < 4 || rook_left_space > 6)
                {
                    continue;
                }
            }
            else
            {
                if(rook_left_space < 2 || rook_left_space > 4)
                {
                    continue;
                }
            }

            board.ascii_draw(WHITE);
            std::cout << std::string(1, castle_side) + "-castle should be illegal here." << std::endl;
            tests_passed = false;
        }
    }


    // Test Genetic_AI file loading
    auto ai1 = Genetic_AI();
    for(int i = 0; i < 1000; ++i)
    {
        ai1.mutate();
    }
    auto file_name1 = "testing/genome1.txt";
    auto file_name2 = "testing/genome2.txt";
    remove(file_name1);
    remove(file_name2);

    ai1.print_genome(file_name1);

    auto ai2 = Genetic_AI(file_name1);
    ai2.print_genome(file_name2);

    if( ! files_are_identical(file_name1, file_name2))
    {
        tests_passed = false;
    }
    remove(file_name1);
    remove(file_name2);


    // Load file from gene pool
    auto gene_pool_file_name = "testing/gene_pool_testing.txt";
    auto gene_pool_expected_file_name = "testing/gene_pool_42.txt";
    auto gene_pool_result_file_name = "testing/gene_pool_result.txt";
    int id = 42;
    try
    {
        auto gai42 = Genetic_AI(gene_pool_file_name, id);
        remove(gene_pool_result_file_name);
        gai42.print_genome(gene_pool_result_file_name);

        if( ! files_are_identical(gene_pool_result_file_name, gene_pool_expected_file_name))
        {
            std::cerr << "Genome loaded from gene pool file not preserved." << std::endl;
            tests_passed = false;
        }
        remove(gene_pool_result_file_name);
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Error reading " << gene_pool_file_name << std::endl;
        std::cerr << e.what() << std::endl;
        tests_passed = false;
    }

    // String utilities
    std::string original = "   a    #     b";
    std::string expected = "a";
    std::string result = String::strip_comments(original, '#');
    if(expected != result)
    {
        std::cerr << "\"" << original << "\" --> \"" << result << "\"" << std::endl;
        tests_passed = false;
    }


    // Poisson distribution check
    std::ofstream ofs("poisson.txt");
    double mean = 5.0;
    for(int v = 0; v <= 3*mean; ++v)
    {
        ofs << v << '\t' << Math::poisson_probability(mean, v) << std::endl;
    }

    std::ofstream ofs2("game_moves.txt");
    double avg_moves_per_game = 40;
    for(int m = 0; m <= 3*avg_moves_per_game; ++m)
    {
        ofs2 << m << '\t' << Math::average_moves_left(avg_moves_per_game, m) << std::endl;
    }


    if(tests_passed)
    {
        std::cout << "All tests passed." << std::endl;
    }
    else
    {
        std::cout << "Tests failed." << std::endl;
    }
}
#else
void run_tests() {}
#endif
