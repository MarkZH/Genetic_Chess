#include "Testing.h"

#include <memory>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Game/Board.h"
#include "Moves/Move.h"
#include "Players/Genetic_AI.h"
#include "Exceptions/Generic_Exception.h"

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
                throw Illegal_Move_Exception("test failed");
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
            throw Illegal_Move_Exception("Test failed.");
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
        throw Generic_Exception("Test failed!");
    }
    remove(file_name1);
    remove(file_name2);


    // Load file from gene pool
    auto gene_pool_file_name = "testing/gene_pool_testing.txt";
    auto gene_pool_expected_file_name = "testing/gene_pool_42.txt";
    auto gene_pool_result_file_name = "testing/gene_pool_result.txt";
    int id = 42;
    auto gai42 = Genetic_AI(gene_pool_file_name, id);
    remove(gene_pool_result_file_name);
    gai42.print_genome(gene_pool_result_file_name);

    if( ! files_are_identical(gene_pool_result_file_name, gene_pool_expected_file_name))
    {
        throw Generic_Exception("Test failed!");
    }
    remove(gene_pool_result_file_name);

    // String utilities
    std::string original = "   a    #     b";
    std::string expected = "a";
    std::string result = String::strip_comments(original, '#');
    if(expected != result)
    {
        std::cerr << "\"" << original << "\" --> \"" << result << "\"" << std::endl;
        throw Generic_Exception("Test failed!");
    }


    std::cout << "All tests passed." << std::endl;
}
