#include "Testing.h"

#ifdef DEBUG
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#include <thread>
#include <iomanip>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Moves/Move.h"
#include "Players/Genetic_AI.h"
#include "Players/Game_Tree_Node_Result.h"

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

    // Basic chess rules check
    Board starting_board;
    auto starting_move_count = starting_board.legal_moves().size();
    size_t correct_move_count = 20;
    if(starting_move_count != correct_move_count)
    {
        std::cerr << "Wrong number of legal moves at beginning of game. Got " << starting_move_count
                  << ", should be " << correct_move_count << std::endl;
        std::cerr << "Legal moves found:" << std::endl;
        auto move_count = 0;
        for(const auto& move : starting_board.legal_moves())
        {
            std::cerr << ++move_count << ". " << move->game_record_item(starting_board) << std::endl;
        }
        starting_board.ascii_draw(WHITE);

        tests_passed = false;
    }

    // Moves that are currently illegal but would land on board
    size_t correct_other_move_count = 86;
    if(starting_board.other_moves().size() != correct_other_move_count)
    {
        std::cerr << "Expected other move count = " << correct_other_move_count << std::endl;
        starting_board.ascii_draw(WHITE);
        auto num = 1;
        for(const auto& move : starting_board.other_moves())
        {
            std::cout << num++ << ". " << move->coordinate_move() << " ";
        }
        std::cout << std::endl;
        tests_passed = false;
    }

    starting_board.submit_move(starting_board.get_move("e4"));
    starting_move_count = starting_board.legal_moves().size();
    if(starting_move_count != correct_move_count)
    {
        std::cerr << "Wrong number of legal moves at beginning of game. Got " << starting_move_count
                  << ", should be " << correct_move_count << std::endl;
        std::cerr << "Legal moves found:" << std::endl;
        auto move_count = 0;
        for(const auto& move : starting_board.legal_moves())
        {
            std::cerr << ++move_count << ". " << move->game_record_item(starting_board) << std::endl;
        }
        starting_board.ascii_draw(WHITE);

        tests_passed = false;
    }

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
                board.get_move('e', 1, final_file, 1);
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


    // Test pawn captures
    Board white_pawn_board("k7/8/8/4p3/3P4/8/8/K7 w - - 0 1");
    try
    {
        white_pawn_board.get_move('d', 4, 'e', 5);
    }
    catch(const Illegal_Move_Exception&)
    {
        white_pawn_board.ascii_draw(WHITE);
        std::cerr << "Number of legal moves = " << white_pawn_board.legal_moves().size() << std::endl;
        std::cerr << "Pawn capture by white should be legal." << std::endl;
        tests_passed = false;
    }

    Board black_pawn_board("k7/8/8/4p3/3P4/8/8/K7 b - - 0 1");
    try
    {
        black_pawn_board.get_move('e', 5, 'd', 4);
    }
    catch(const Illegal_Move_Exception&)
    {
        black_pawn_board.ascii_draw(WHITE);
        std::cerr << "Number of legal moves = " << black_pawn_board.legal_moves().size() << std::endl;
        std::cerr << "Pawn capture by white should be legal." << std::endl;
        tests_passed = false;
    }


    // Test Genetic_AI file loading
    auto pool_file_name = "test_gene_pool.txt";
    auto write_file_name = "test_genome_write.txt";
    auto rewrite_file_name = "test_genome_rewrite.txt";
    remove(pool_file_name);
    remove(write_file_name);
    remove(rewrite_file_name);

    std::vector<Genetic_AI> test_pool(10);
    for(auto& ai : test_pool)
    {
        for(int i = 0; i < 1000; ++i)
        {
            ai.mutate();
        }
        ai.print_genome(pool_file_name);
    }

    auto index = Random::random_integer(0, test_pool.size() - 1);
    test_pool[index].print_genome(write_file_name);
    auto read_ai = Genetic_AI(pool_file_name, index);
    read_ai.print_genome(rewrite_file_name);

    if( ! files_are_identical(write_file_name, rewrite_file_name))
    {
        std::cerr << "Genome loaded from gene pool file not preserved." << std::endl;
        tests_passed = false;
    }
    else
    {
        remove(pool_file_name);
        remove(write_file_name);
        remove(rewrite_file_name);
    }


    // String utilities
    std::string original = "   a    #     b";
    std::string expected = "a";
    std::string result = String::strip_comments(original, '#');
    if(expected != result)
    {
        std::cerr << "\"" << original << "\" --> \"" << result << "\"" << std::endl;
        std::cerr << "Expected result: \"" << expected << "\"" << std::endl;
        tests_passed = false;
    }

    original = "   a    {    b    }    c   {   d  }   ";
    expected = "a c";
    result = String::strip_block_comment(original, '{', '}');
    if(expected != result)
    {
        std::cerr << "\"" << original << "\" --> \"" << result << "\"" << std::endl;
        std::cerr << "Expected result: \"" << expected << "\"" << std::endl;
        tests_passed = false;
    }

    std::string search_string = "abcdefg";
    std::string target = "abc";
    if( ! String::starts_with(search_string, target))
    {
        std::cerr << "\"" << target << "\" not found at beginning of " << search_string << std::endl;
        tests_passed = false;
    }

    std::string wrong_target = "abd";
    if(String::starts_with(search_string, wrong_target))
    {
        std::cerr << "\"" << wrong_target << "\" erroneously found at beginning of " << search_string << std::endl;
        tests_passed = false;
    }


    // Log-Norm distribution check
    const double mean_moves = 26.0;
    const double width = .5;
    const size_t moves_so_far = 22;
    auto moves_left = Math::average_moves_left(mean_moves, width, moves_so_far);
    auto expected_moves_left = 15.2629;
    if(std::abs(moves_left - expected_moves_left) > 1e-4)
    {
        std::cout << "Log-Norm failed: Expected: " << expected_moves_left
                  << " --- Got: " << moves_left << std::endl;
        tests_passed = false;
    }


    // Clock time reset test
    auto time = 30;
    double expected_time_after_reset;
    auto moves_to_reset = 40;
    auto clock = Clock(time, moves_to_reset);
    clock.start();
    for(int i = 0; i < 2*moves_to_reset; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        expected_time_after_reset = clock.time_left(BLACK) + time;
        clock.punch();
    }
    clock.stop();
    if(std::abs(clock.time_left(BLACK) - expected_time_after_reset) > 1e-3)
    {
        std::cerr << "Clock incorrect: time left for black is " << clock.time_left(BLACK) << " sec. Should be " << expected_time_after_reset << "sec." << std::endl;
        tests_passed = false;
    }

    // Clock time increment test
    auto increment = 5;
    auto clock2 = Clock(time, 0, increment);
    clock2.start();
    double expected_time = time;
    for(int i = 0; i < 2*moves_to_reset; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        clock2.punch();
        if(i % 2 == 1) // only on black moves
        {
            expected_time += increment - 0.005;
        }
    }
    clock2.stop();
    if(std::abs(clock2.time_left(BLACK) - expected_time) > 0.01)
    {
        std::cerr << "Clock incorrect: time left for black is " << clock2.time_left(BLACK) << " sec. Should be " << expected_time << "sec." << std::endl;
        tests_passed = false;
    }

    Game_Tree_Node_Result r1 = {10,
                                WHITE,
                                2,
                                {}};
    Game_Tree_Node_Result r2 = {10,
                                BLACK,
                                2,
                                {}};

    if(better_than(r2, r1, WHITE))
    {
        std::cerr << "1. Error in comparing Game Tree Node Results." << std::endl;
        tests_passed = false;
    }

    if(better_than(r1, r2, BLACK))
    {
        std::cerr << "2. Error in comparing Game Tree Node Results." << std::endl;
        tests_passed = false;
    }

    Game_Tree_Node_Result alpha_start = {Math::lose_score,
                                         WHITE,
                                         0,
                                         {}};

    Game_Tree_Node_Result beta_start = {Math::win_score,
                                        WHITE,
                                        0,
                                        {}};
    if(better_than(alpha_start, beta_start, WHITE))
    {
        std::cerr << "3. Error in comparing Game Tree Node Results." << std::endl;
        tests_passed = false;
    }

    if( ! better_than(alpha_start, beta_start, BLACK))
    {
        std::cerr << "4. Error in comparing Game Tree Node Results." << std::endl;
        tests_passed = false;
    }


    Game_Tree_Node_Result white_win4 = {Math::win_score,
                                        WHITE,
                                        4,
                                        {}};
    Game_Tree_Node_Result white_win6 = {Math::win_score,
                                        WHITE,
                                        6,
                                        {}};
    if(better_than(white_win6, white_win4, WHITE))
    {
        std::cerr << "Later win preferred over earlier win." << std::endl;
        tests_passed = false;
    }

    if(better_than(white_win4, white_win6, BLACK))
    {
        std::cerr << "Earlier loss preferred over later win." << std::endl;
        tests_passed = false;
    }

    Game_Tree_Node_Result black_loss6 = {Math::lose_score,
                                         BLACK,
                                         6,
                                         {}};
    if( ! (white_win6 == black_loss6))
    {
        std::cerr << "White win in 6 not equal to black loss in 6." << std::endl;
        tests_passed = false;
    }


    // Move ambiguity check
    Board board;
    bool ambiguous_move_caught = false;
    try
    {
        for(auto move : {"Nc3", "a1",
                         "Nf3", "b1",
                         "Ne4", "c1",
                         "Ng5"})
        {
            board.submit_move(board.get_move(move));
        }
    }
    catch(const Illegal_Move_Exception&)
    {
        ambiguous_move_caught = true;
    }

    if( ! ambiguous_move_caught)
    {
        std::cerr << "Last move should have been an error:" << std::endl;
        board.print_game_record(nullptr, nullptr,
                                "", Game_Result(NONE, "", false), 0, 0, 0, Clock());

        tests_passed = false;
    }

    // check square colors are correct
    Board().ascii_draw(WHITE);

    int int_width = 10;
    int real_width = 15;
    int norm_width = 15;
    std::cout << std::setw(int_width) << "Integers"
              << std::setw(real_width) << "Reals"
              << std::setw(norm_width) << "Normals" << '\n';
    std::cout << std::setw(int_width) << "+/-1000"
              << std::setw(real_width) << "[0-2]"
              << std::setw(norm_width) << "sig = 3" << '\n';
    for(int i = 0; i < 10; ++i)
    {
        std::cout << std::setw(int_width) << Random::random_integer(-1000, 1000)
                  << std::setw(real_width) << Random::random_real(0, 2)
                  << std::setw(norm_width) << Random::random_normal(3) << '\n';
    }

    // FEN input/output
    for(std::string test : {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                            "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
                            "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2",
                            "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"})
    {
        if(Board(test).fen_status() != test)
        {
            std::cerr << test << " -->\n" << Board(test).fen_status() << "\n\n";
            tests_passed = false;
        }
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
