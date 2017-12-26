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

#include "Genes/Castling_Possible_Gene.h"
#include "Genes/Freedom_To_Move_Gene.h"
#include "Genes/King_Confinement_Gene.h"
#include "Genes/King_Protection_Gene.h"
#include "Genes/Opponent_Pieces_Targeted_Gene.h"
#include "Genes/Pawn_Advancement_Gene.h"
#include "Genes/Piece_Strength_Gene.h"
#include "Genes/Sphere_of_Influence_Gene.h"
#include "Genes/Total_Force_Gene.h"

#include "Utility.h"

#include "Exceptions/Illegal_Move_Exception.h"

// Declaration to silence warnings
bool files_are_identical(const std::string& file_name1, const std::string& file_name2);
size_t move_count(const Board& board, size_t maximum_depth, const std::string& file_name);

void run_tests()
{
    bool tests_passed = true;

    // Basic chess rules check
    Board starting_board;
    while(true)
    {
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

            if(num == 1)
            {
                std::cout << "No other moves found.";
            }

            std::cout << std::endl;
            tests_passed = false;
        }

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

            if(move_count == 0)
            {
                std::cout << "No legal moves found.\n";
            }

            starting_board.ascii_draw(WHITE);

            tests_passed = false;
        }

        if(starting_board.whose_turn() == BLACK)
        {
            break;
        }

        starting_board.set_turn(BLACK);
    }

    Board second_move_board;
    second_move_board.submit_move(second_move_board.get_move("e4"));
    auto second_move_count = second_move_board.legal_moves().size();
    size_t correct_second_move_count = 20;
    if(second_move_count != correct_second_move_count)
    {
        std::cerr << "Wrong number of legal moves at beginning of game. Got " << second_move_count
                  << ", should be " << correct_second_move_count << std::endl;
        std::cerr << "Legal moves found:" << std::endl;
        auto move_count = 0;
        for(const auto& move : second_move_board.legal_moves())
        {
            std::cerr << ++move_count << ". " << move->game_record_item(second_move_board) << std::endl;
        }
        second_move_board.ascii_draw(WHITE);

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


    // Prove there was a problem with generating moves before move
    // side effects are applied to a board.

    //   abcdefgh
    // 8 ..k.....
    // 7 P.......
    // 6 K.......
    // White pawn to promote to Queen
    auto side_effects_board = Board("2k7/P7/K/8/8/8/8/8 w - - 0 1");
    side_effects_board.submit_move(side_effects_board.get_move("a8=Q"));
    auto illegal_move_made = true;
    try
    {
        side_effects_board.get_move("Kb8");
    }
    catch(const Illegal_Move_Exception&)
    {
        illegal_move_made = false;
    }

    if(illegal_move_made)
    {
        side_effects_board.ascii_draw(WHITE);
        std::cerr << "Kb7 should not be legal here." << std::endl;
        tests_passed = false;
    }


    // Pinned piece test
    auto pin_board = Board("k1KRr3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! pin_board.piece_is_pinned('d', 8))
    {
        pin_board.ascii_draw(WHITE);
        std::cerr << "Rook at d4 should register as pinned." << std::endl;
        tests_passed = false;
    }

    auto no_pin_board = Board("k1KRRr2/8/8/8/8/8/8/8 w - - 0 1");
    if(no_pin_board.piece_is_pinned('d', 8))
    {
        no_pin_board.ascii_draw(WHITE);
        std::cerr << "Rook at d4 should not register as pinned." << std::endl;
        tests_passed = false;
    }

    auto en_passant_pin_board = Board("K7/8/8/8/kpP4R/8/8/8 b - c3 0 1");
    auto move_string = "b4c3";
    auto move_is_legal = true;
    try
    {
        en_passant_pin_board.get_move(move_string);
    }
    catch(const Illegal_Move_Exception&)
    {
        move_is_legal = false;
    }

    if(move_is_legal)
    {
        en_passant_pin_board.ascii_draw(WHITE);
        std::cerr << "En passant capture by black (" << move_string << ") should not be legal here." << std::endl;
        tests_passed = false;
    }

    Board perf_board;
    try
    {
        for(const auto& move : {"c3", "a6", "Qa4"})
        {
            perf_board.submit_move(perf_board.get_move(move));
        }
    }
    catch(const Illegal_Move_Exception&)
    {
        perf_board.ascii_draw(WHITE);
        perf_board.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
        std::cerr << "All moves so far should have been legal." << std::endl;
        tests_passed = false;
    }

    bool legal_move = true;
    auto illegal_move = "d5";
    try
    {
        perf_board.submit_move(perf_board.get_move(illegal_move));
    }
    catch(const Illegal_Move_Exception&)
    {
        legal_move = false;
    }

    if(legal_move)
    {
        perf_board.ascii_draw(WHITE);
        std::cout << "Last move (" << illegal_move << ") should have been illegal." << std::endl;
        tests_passed = false;
    }


    Board perf_board2;
    auto moves2 = {"c3", "d6", "Qa4", "Nc6"};
    try
    {
        for(const auto& move : moves2)
        {
            perf_board2.submit_move(perf_board2.get_move(move));
        }
    }
    catch(const Illegal_Move_Exception&)
    {
        perf_board2.ascii_draw(WHITE);
        perf_board2.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
        for(const auto& move : moves2)
        {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        std::cerr << "All moves so far should have been legal." << std::endl;
        tests_passed = false;
    }

    Board perf_board3;
    std::vector<std::string> moves3 = {"d3", "c6", "Bd2", "Qa5", "Bb4"};
    try
    {
        for(const auto& move : moves3)
        {
            perf_board3.submit_move(perf_board3.get_move(move));
        }
    }
    catch(const Illegal_Move_Exception&)
    {
        perf_board3.ascii_draw(WHITE);
        perf_board3.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
        for(const auto& move : moves3)
        {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        std::cerr << "All moves so far should have been legal." << std::endl;
        tests_passed = false;
    }

    Board perf_board4;
    std::vector<std::string> moves4 = {"e3", "a6", "Qe2"};
    try
    {
        Game_Result result;
        for(const auto& move : moves4)
        {
            result = perf_board4.submit_move(perf_board4.get_move(move));
        }

        if(result.game_has_ended())
        {
            perf_board4.ascii_draw(WHITE);
            std::cerr << "This is not checkmate." << std::endl;
            tests_passed = false;
        }
    }
    catch(const Illegal_Move_Exception&)
    {
        perf_board4.ascii_draw(WHITE);
        perf_board4.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
        for(const auto& move : moves4)
        {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        std::cerr << "All moves so far should have been legal." << std::endl;
        tests_passed = false;
    }


    // Test Genetic_AI file loading
    std::cout << "Testing genome file handling ... " << std::flush;
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
    std::cout << "Done." << std::endl;


    // Test individual board-scoring genes
    auto test_genes_file_name = "test_genome.txt";

    auto castling_possible_gene = Castling_Possible_Gene();
    castling_possible_gene.read_from(test_genes_file_name);
    auto castling_board = Board("rn2k4/8/8/8/8/8/8/R3K2R w KQq - 0 1");
    auto white_castling_score = 0.8*(3.0/4.0) + 0.2*(4.0/5.0); // maximum score with and without actually castling
    tests_passed &= castling_possible_gene.test(castling_board, white_castling_score);

    castling_board.submit_move(castling_board.get_move("O-O"));
    auto black_castling_score = 0.2*(3.0/5.0); // castling possible
    tests_passed &= castling_possible_gene.test(castling_board, black_castling_score);

    castling_board.submit_move(castling_board.get_move("Nc6"));
    tests_passed &= castling_possible_gene.test(castling_board, 1.0);

    auto freedom_to_move_gene = Freedom_To_Move_Gene();
    auto freedom_to_move_board = Board("5k2/8/8/8/4Q3/8/8/3K4 w - - 0 1");
    auto freedom_to_move_score = 32.0/20.0;
    tests_passed &= freedom_to_move_gene.test(freedom_to_move_board, freedom_to_move_score);

    auto king_confinement_gene = King_Confinement_Gene();
    auto king_confinement_board = Board("k3r3/8/8/8/8/8/5PPP/7K w - - 0 1");
    auto king_confinement_score = (1.0 + 1.0/2.0 + 1.0/3.0)/king_confinement_gene.get_maximum_score();
    tests_passed &= king_confinement_gene.test(king_confinement_board, king_confinement_score);

    auto king_protection_gene = King_Protection_Gene();
    auto king_protection_board = king_confinement_board;
    auto max_square_count = 8 + 7 + 7 + 7 + 6; // max_square_count in King_Protection_Gene.cpp
    auto square_count = 7 + 1; // row attack along rank 1 + knight attack from g3
    auto king_protection_score = double(max_square_count - square_count)/max_square_count;
    tests_passed &= king_protection_gene.test(king_protection_board, king_protection_score);

    auto piece_strength_gene = Piece_Strength_Gene();
    piece_strength_gene.read_from(test_genes_file_name);
    auto piece_strength_normalizer = double(16 + 2*8 + 2*4 + 2*2 + 8*1);

    auto opponent_pieces_targeted_gene = Opponent_Pieces_Targeted_Gene(&piece_strength_gene);
    auto opponent_pieces_targeted_board = Board("k1K5/8/8/8/8/1rp5/nQb5/1q6 w - - 0 1");
    auto opponent_pieces_targeted_score = (16 + 8 + 4 + 2 + 1)/piece_strength_normalizer;
    tests_passed &= opponent_pieces_targeted_gene.test(opponent_pieces_targeted_board, opponent_pieces_targeted_score);

    auto pawn_advancement_gene = Pawn_Advancement_Gene();
    auto pawn_advancement_board = Board("7k/4P3/3P4/2P5/1P6/P7/8/K7 w - - 0 1");
    auto pawn_advancement_score = double(1 + 2 + 3 + 4 + 5)/(8*6);
    pawn_advancement_gene.test(pawn_advancement_board, pawn_advancement_score);

    auto sphere_of_influence_gene = Sphere_of_Influence_Gene();
    sphere_of_influence_gene.read_from(test_genes_file_name);
    auto sphere_of_influence_board = Board("k7/8/8/8/1R3p2/8/8/K7 w - - 0 1");
    auto sphere_of_influence_score
        = (4.0 * (1 + (2.0/(1 + 1.0))))  // b8
        + (4.0 * (1 + (2.0/(1 + 1.0))))  // b7
        + (4.0 * (1 + (2.0/(1 + 2.0))))  // b6
        + (4.0 * (1 + (2.0/(1 + 3.0))))  // b5
        + (4.0 * (1 + (2.0/(1 + 5.0))))  // b3
        + (4.0 * (1 + (2.0/(1 + 6.0))))  // b2
        + (4.0 * (1 + (2.0/(1 + 7.0))))  // b1
        + (4.0 * (1 + (2.0/(1 + 4.0))))  // a4
        + (4.0 * (1 + (2.0/(1 + 6.0))))  // a2
        + (4.0 * (1 + (2.0/(1 + 4.0))))  // c4
        + (4.0 * (1 + (2.0/(1 + 4.0))))  // d4
        + (4.0 * (1 + (2.0/(1 + 4.0))))  // e4
        + (4.0 * (1 + (2.0/(1 + 5.0))))  // f4
        + (1.0 * (1 + (2.0/(1 + 6.0))))  // g4
        + (1.0 * (1 + (2.0/(1 + 7.0)))); // h4
    sphere_of_influence_score /= 64;
    // Setup       Square score     King distance (from black king)
    // k.......    k4......         k1......
    // ........    .4......         .1......
    // ........    .4......         .2......
    // ........    .4......         .3......
    // .R...p..    4R444411         4R444567
    // ........    .4......         .5......
    // ........    44......         66......
    // K.......    K4......         K7......
    sphere_of_influence_gene.test(sphere_of_influence_board, sphere_of_influence_score);

    auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
    tests_passed &= total_force_gene.test(Board(), 1.0 + 32/piece_strength_normalizer);


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

    std::string splitting_line = "\t a b c d e ";
    std::vector<std::string> expected_split_line = {"a", "b", "c", "d", "e"};
    auto split_line = String::split(splitting_line);
    if(split_line.size() != expected_split_line.size() || ! std::equal(split_line.begin(), split_line.end(), expected_split_line.begin()))
    {
        std::cerr << "These lists should match from line splitting:\nExected from String::split(" << splitting_line << ")\n";
        for(const auto& thing : expected_split_line)
        {
            std::cerr << thing << ", ";
        }
        std::cerr << "\nReturned by String::split()\n";
        for(const auto& thing : split_line)
        {
            std::cerr << thing << ", ";
        }
        std::cerr << std::endl;

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
    double expected_time_after_reset = 0.0;
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
                                {nullptr, nullptr, nullptr}};
    Game_Tree_Node_Result r2 = {10,
                                BLACK,
                                {nullptr, nullptr, nullptr}};

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
                                         {nullptr}};

    Game_Tree_Node_Result beta_start = {Math::win_score,
                                        WHITE,
                                        {nullptr}};
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
                                        {nullptr, nullptr, nullptr,
                                         nullptr, nullptr}};
    Game_Tree_Node_Result white_win6 = {Math::win_score,
                                        WHITE,
                                        {nullptr, nullptr, nullptr,
                                         nullptr, nullptr, nullptr,
                                         nullptr}};
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
                                         {nullptr, nullptr, nullptr,
                                          nullptr, nullptr, nullptr,
                                          nullptr}};
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

    // Count game tree leaves to given depth (http://oeis.org/A048987)
    auto ply_counts = std::map<size_t, size_t>{{0, 1},
                                               {1, 20},
                                               {2, 400},
                                               {3, 8902}};
                                               //{4, 197281},
                                               //{5, 4865609}};
    for(const auto& depth_expected : ply_counts)
    {
        auto plies = depth_expected.first;
        auto answer = depth_expected.second;
        std::cout << "Counting moves to " << plies << "-ply depth ... " << std::flush;
        std::string file_name = "";
        size_t count = move_count(Board(), plies, file_name);
        if(count != answer)
        {
            std::cerr << "\nExpected game tree leaves: " << count << "  Got: " << answer << std::endl;
            tests_passed = false;
        }
        std::cout << "Done." << std::endl;
    }

    // check square colors are correct
    auto current_color = WHITE;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        current_color = opposite(current_color);
        for(int rank = 1; rank <= 8; ++rank)
        {
            if(Board::square_color(file, rank) != current_color)
            {
                std::cerr << "Wrong color for square " << file << rank
                          << ". Should be " << color_text(current_color) << '\n';
                tests_passed = false;
            }
            current_color = opposite(current_color);
        }
    }

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
    for(std::string test : {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Start
                            "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1", // 1. e4
                            "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2",  // 1. ... c5
                            "rnbqkbnr/pp1ppppp/8/2p1P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2",  // 2. e5
                            "rnbqkbnr/pp1pp1pp/8/2p1Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3"}) // 2. ... f4
    {
        auto board_fen = Board(test);
        if(board_fen.fen_status() != test)
        {
            std::cerr << test << " -->\n" << board_fen.fen_status() << "\n\n";
            board_fen.ascii_draw();
            tests_passed = false;
        }
    }

    Board capture_board;
    capture_board.submit_move(capture_board.get_move("e4"));
    capture_board.submit_move(capture_board.get_move("d5"));
    if( ! capture_board.capture_possible())
    {
        capture_board.ascii_draw(WHITE);
        std::cerr << "Capture should be possible here." << std::endl;
        tests_passed = false;
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

size_t move_count(const Board& board, size_t maximum_depth, const std::string& file_name)
{
    if(maximum_depth == 0)
    {
        if( ! file_name.empty())
        {
            board.print_game_record(nullptr, nullptr, file_name, {}, 0, 0, 0, {});
        }
        return 1;
    }

    size_t count = 0;
    for(auto move : board.legal_moves())
    {
        if(board.get_game_record().empty())
        {
            std::cout << move->game_record_item(board) << ' ';
        }
        auto next_board = board;
        next_board.submit_move(*move);
        count += move_count(next_board, maximum_depth - 1, file_name);
    }

    return count;
}

#else
void run_tests() {}
#endif
