#include "Testing.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#include <thread>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include <limits>
#include <utility>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"
#include "Game/Square.h"
#include "Moves/Move.h"

#include "Players/Genetic_AI.h"
#include "Players/Game_Tree_Node_Result.h"

#include "Genes/Castling_Possible_Gene.h"
#include "Genes/Freedom_To_Move_Gene.h"
#include "Genes/King_Confinement_Gene.h"
#include "Genes/King_Protection_Gene.h"
#include "Genes/Opponent_Pieces_Targeted_Gene.h"
#include "Genes/Pawn_Advancement_Gene.h"
#include "Genes/Passed_Pawn_Gene.h"
#include "Genes/Piece_Strength_Gene.h"
#include "Genes/Sphere_of_Influence_Gene.h"
#include "Genes/Total_Force_Gene.h"
#include "Genes/Stacked_Pawns_Gene.h"
#include "Genes/Pawn_Islands_Gene.h"
#include "Genes/Checkmate_Material_Gene.h"
#include "Genes/Null_Gene.h"

#include "Utility/String.h"
#include "Utility/Scoped_Stopwatch.h"
#include "Utility/Random.h"
#include "Utility/Math.h"

#include "Exceptions/Illegal_Move.h"

namespace
{
    bool files_are_identical(const std::string& file_name1, const std::string& file_name2);
    size_t move_count(const Board& board, size_t maximum_depth);
    bool run_board_tests(const std::string& file_name);
    bool all_moves_legal(Board& board, const std::vector<std::string>& moves);
    bool move_is_illegal(Board& board, const std::string& moves);
}

bool run_tests()
{
    bool tests_passed = true;

    // Move direction indexing
    for(size_t i = 0; i < 16; ++i)
    {
        auto step = Move::attack_direction_from_index(i);
        auto start = Square{'e', 4};
        auto step_index = Move(start, start + step).attack_index();
        if(step_index != i)
        {
            std::cerr << "Direction-index mismatch: " << i << " --> " << step.file_change << "," << step.rank_change << " --> " << step_index << std::endl;
            tests_passed = false;
        }
    }


    // Square indexing tests
    std::array<bool, 64> visited{};
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto square = Square{file, rank};
            if(visited[square.index()])
            {
                std::cerr << "Multiple squares result in same index." << std::endl;
                tests_passed = false;
            }
            visited[square.index()] = true;
            auto indexed_square = Square(square.file(), square.rank());
            if(square != indexed_square)
            {
                std::cerr << "Incorrect square indexing.\n";
                std::cerr << file << rank << " --> " << square.index() << " --> " << indexed_square.file() << indexed_square.rank() << std::endl;
                tests_passed = false;
            }
        }
    }

    if( ! std::all_of(visited.begin(), visited.end(), [](auto x){ return x; }))
    {
        std::cerr << "Not all indices visited by iterating through all squares." << std::endl;
        tests_passed = false;
    }


    // Piece construction tests
    for(auto type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
    {
        for(auto color : {BLACK, WHITE})
        {
            auto piece = Piece{color, type};
            auto piece2 = Piece{piece.color(), piece.type()};
            if(piece != piece2)
            {
                std::cerr << "Inconsistent construction for " << piece.fen_symbol() << " --> " << piece2.fen_symbol() << std::endl;
                tests_passed = false;
            }
        }
    }


    // Check that square colors are correct
    auto current_color = WHITE;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        current_color = opposite(current_color);
        for(int rank = 1; rank <= 8; ++rank)
        {
            if(Square{file, rank}.color() != current_color)
            {
                std::cerr << "Wrong color for square " << file << rank
                          << ". Should be " << color_text(current_color) << '\n';
                tests_passed = false;
            }
            current_color = opposite(current_color);
        }
    }


    // Check that Square arithmetic works
    for(auto a : Square::all_squares())
    {
        for(auto b : Square::all_squares())
        {
            if(a + (b - a) != b)
            {
                std::cerr << "Square arithetic problem: " << a.string()
                    << " + (" << b.string() << " - " << a.string()
                    << ") != " << b.string() << std::endl;
                tests_passed = false;
            }
        }
    }

    // Check square iteration
    std::array<bool, 64> squares_visited{};
    for(auto square : Square::all_squares())
    {
        if(squares_visited[square.index()])
        {
            std::cerr << "Sqaure " << square.string() << " already visited." << std::endl;
            tests_passed = false;
        }
    }
    if(std::any_of(squares_visited.begin(), squares_visited.end(), [](auto tf) {return tf; }))
    {
        std::cerr << "Square iterator missed some squares." << std::endl;
        tests_passed = false;
    }


    // Threefold repetition rule test
    auto repeat_board = Board();
    Game_Result repeat_result;
    auto repeat_move_count = 0;

    // Repeating the sequence of moves twice creates a board with the starting position
    // three times (including the state before any moves).
    for(int repetition = 0; repetition < 2; ++repetition)
    {
        for(const auto& move : {"Nc3", "Nc6", "Nb1", "Nb8"})
        {
            ++repeat_move_count;
            repeat_result = repeat_board.submit_move(repeat_board.create_move(move));
            if(repeat_result.game_has_ended())
            {
                if(repeat_move_count < 8)
                {
                    tests_passed = false;
                    std::cerr << "Threefold repetition triggered early." << std::endl;
                }
            }
        }
    }

    if( ! repeat_result.game_has_ended())
    {
        tests_passed = false;
        std::cerr << "Threefold stalemate not triggered." << std::endl;
    }
    else if(repeat_result.ending_reason() != "Threefold repetition")
    {
        tests_passed = false;
        std::cerr << "Wrong game ending. Got " << repeat_result.ending_reason() << "; Expected Threefold Repetition." << std::endl;
    }


    // Fifty-move stalemate test (100 plies with no capture or pawn movement)
    auto fifty_move_board = Board();
    auto fifty_move_result = Game_Result();
    for(int move_counter = 1; move_counter <= 100; ++move_counter)
    {
        auto move_chosen = false;
        for(const auto& move : fifty_move_board.legal_moves())
        {
            if(fifty_move_board.move_captures(*move))
            {
                continue;
            }

            const auto& fifty_move_board_view = fifty_move_board;
            if(fifty_move_board_view.piece_on_square(move->start()).type() == PAWN)
            {
                continue;
            }

            auto next_board = fifty_move_board;
            auto result = next_board.submit_move(*move);

            if(result.winner() != NONE)
            {
                continue;
            }

            if(result.game_has_ended())
            {
                if(result.ending_reason() == "Threefold repetition")
                {
                    continue;
                }

                if(move_counter < 100)
                {
                    tests_passed = false;
                    std::cerr << "Fifty-move statlemate triggered early." << std::endl;
                    std::cout << result.ending_reason() << std::endl;
                    break;
                }
            }

            fifty_move_result = fifty_move_board.submit_move(*move);
            move_chosen = true;
            break;
        }

        if( ! move_chosen)
        {
            tests_passed = false;
            std::cerr << "Unable to choose next move (moves made = " << move_counter << ")." << std::endl;
            fifty_move_board.ascii_draw(WHITE);
            break;
        }
    }

    if(fifty_move_result.ending_reason() != "50-move limit")
    {
        tests_passed = false;
        std::cerr << "Error in 50-move stalemate test." << std::endl;
        std::cerr << "Got: " << fifty_move_result.ending_reason() << "; Expected 50-move limit." << std::endl;
    }


    if( ! run_board_tests("testing/board_tests.txt"))
    {
        tests_passed = false;
    }


    // Last move captured/changed material tests
    Board capture_board;
    capture_board.submit_move(capture_board.create_move("b4"));
    capture_board.submit_move(capture_board.create_move("c5"));
    if( ! capture_board.material_change_possible())
    {
        capture_board.ascii_draw(WHITE);
        std::cerr << "This board has a capturing move." << std::endl;
        tests_passed = false;
    }
    capture_board.submit_move(capture_board.create_move("bxc5"));
    if( ! capture_board.last_move_captured())
    {
        capture_board.ascii_draw(WHITE);
        capture_board.print_game_record(nullptr, nullptr, "", {}, {});
        std::cerr << "The previous move was a capture." << std::endl;
        tests_passed = false;
    }

    if(capture_board.material_change_possible())
    {
        capture_board.ascii_draw(WHITE);
        std::cerr << "This board does not have a capturing move." << std::endl;
        tests_passed = false;
    }
    capture_board.submit_move(capture_board.create_move("h5"));
    if(capture_board.last_move_captured())
    {
        capture_board.ascii_draw(WHITE);
        capture_board.print_game_record(nullptr, nullptr, "", {}, {});
        std::cerr << "The previous move did not capture." << std::endl;
        tests_passed = false;
    }


    // Board hash with castling tests
    auto castling_hash_board = Board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

    // Lose all castling rights due to king moving
    auto just_kings_move_board = castling_hash_board;
    for(auto move : {"Ke2", "Ke7", "Ke1", "Ke8"})
    {
        just_kings_move_board.submit_move(just_kings_move_board.create_move(move));
    }

    // Lose all castling rights due to rooks moving
    auto just_rooks_move_board = castling_hash_board;
    for(auto move : {"Ra2", "Ra7", "Ra1", "Ra8", "Rh2", "Rh7", "Rh1", "Rh8"})
    {
        just_rooks_move_board.submit_move(just_rooks_move_board.create_move(move));
    }

    if(just_kings_move_board.board_hash() != just_rooks_move_board.board_hash())
    {
        std::cerr << "Board hashes differ after castling-canceling moves." << std::endl;
        tests_passed = false;
    }

    if(castling_hash_board.board_hash() == just_kings_move_board.board_hash())
    {
        std::cerr << "Board hashes should be different after castling rights are gone." << std::endl;
        tests_passed = false;
    }


    // Test Genetic_AI file loading
    auto pool_file_name = "test_gene_pool.txt";
    auto write_file_name = "test_genome_write.txt";
    auto rewrite_file_name = "test_genome_rewrite.txt";
    remove(pool_file_name);
    remove(write_file_name);
    remove(rewrite_file_name);

    std::vector<Genetic_AI> test_pool;
    for(int i = 0; i < 10; ++i)
    {
        test_pool.emplace_back(100); // Add AI with 100 mutations
        test_pool.back().set_origin_pool(Random::random_index(9));
    }
    for(auto& ai : test_pool)
    {
        ai = Genetic_AI(ai, Random::random_element(test_pool)); // test ancestry writing/parsing
        ai.print(pool_file_name);
    }

    const auto& test_ai = Random::random_element(test_pool);
    test_ai.print(write_file_name);
    auto read_ai = Genetic_AI(pool_file_name, test_ai.id());
    read_ai.print(rewrite_file_name);

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

    auto genetic_ai_example_file_name = "genetic_ai_example.txt";
    try
    {
        auto example_genetic_ai = Genetic_AI(genetic_ai_example_file_name);
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n" << e.what() << std::endl;
        std::cerr << "Could not load " << genetic_ai_example_file_name << std::endl;
        tests_passed = false;
    }


    // Test individual board-scoring genes
    auto test_genes_file_name = "testing/test_genome.txt";

    auto castling_possible_gene = Castling_Possible_Gene();
    castling_possible_gene.read_from(test_genes_file_name);
    auto castling_board = Board("rn2k3/8/8/8/8/8/8/R3K2R w KQq - 0 1");
    auto white_castling_score = 0.8*(5.0/6.0) + 0.2*(6.0/7.0); // maximum score with and without actually castling
    tests_passed &= castling_possible_gene.test(castling_board, WHITE, white_castling_score);

    castling_board.submit_move(castling_board.create_move("O-O"));
    tests_passed &= castling_possible_gene.test(castling_board, WHITE, 1.0); // full score for kingside castling

    auto black_castling_score = 0.2*(5.0/7.0); // castling possible
    tests_passed &= castling_possible_gene.test(castling_board, BLACK, black_castling_score);

    castling_board.submit_move(castling_board.create_move("Nc6"));
    tests_passed &= castling_possible_gene.test(castling_board, WHITE, 0.0); // castling no longer relevant

    auto freedom_to_move_gene = Freedom_To_Move_Gene();
    auto freedom_to_move_board = Board("5k2/8/8/8/4Q3/8/8/3K4 w - - 0 1");
    auto freedom_to_move_score = 32.0/18.0;
    tests_passed &= freedom_to_move_gene.test(freedom_to_move_board, WHITE, freedom_to_move_score);

    #ifdef NDEBUG
    auto test_move_count = 1'000'000;
    #else
    auto test_move_count = 1'000; // Debug build is approximately 1,000x slower.
    #endif // NDEBUG
    Board freedom_to_move_punishment_board;
    for(auto move_count = 0; move_count < test_move_count; ++move_count)
    {
        if( ! freedom_to_move_gene.verify(freedom_to_move_punishment_board))
        {
            std::cerr << "Attack count discrepancy." << std::endl;
            tests_passed = false;
            break;
        }

        const auto& moves = freedom_to_move_punishment_board.legal_moves();
        auto move = Random::random_element(moves);
        if(freedom_to_move_punishment_board.submit_move(*move).game_has_ended())
        {
            freedom_to_move_punishment_board = Board();
        }
    }

    auto king_confinement_gene = King_Confinement_Gene();
    king_confinement_gene.read_from(test_genes_file_name);
    auto king_confinement_board = Board("k3r3/8/8/8/8/8/5PPP/7K w - - 0 1");
    auto king_confinement_score = (4*(1.0 + 1.0 + 1.0) + // blocked by friendlies (h2, g2, f2)
                                   (-1)*(1.0 + 1.0))/ // blocked by opponent (e1, e2)
                                   (4 + 1)/ // normalizing
                                   (1 + (1 + 1 + 1)); // free squares (h1, g1, f1)
    tests_passed &= king_confinement_gene.test(king_confinement_board, WHITE, king_confinement_score);

    auto king_confined_by_pawns_board = Board("k7/8/8/8/8/pppppppp/8/K7 w - - 0 1");
    auto king_confined_by_pawns_score = (4*(0.0) + // no friendly blockers
                                         (-1)*(8.0))/ // blocked by pawn attacks on second rank
                                         (4 + 1)/ // normalizing
                                         (1 + 8); // free squares (a1-h1)
    tests_passed &= king_confinement_gene.test(king_confined_by_pawns_board, WHITE, king_confined_by_pawns_score);

    auto king_protection_gene = King_Protection_Gene();
    auto king_protection_board = king_confinement_board;
    auto max_square_count = 8 + 7 + 7 + 7 + 6; // max_square_count in King_Protection_Gene.cpp
    auto square_count = 7 + 1; // row attack along rank 1 + knight attack from g3
    auto king_protection_score = double(max_square_count - square_count)/max_square_count;
    tests_passed &= king_protection_gene.test(king_protection_board, WHITE, king_protection_score);

    auto piece_strength_gene = Piece_Strength_Gene();
    piece_strength_gene.read_from(test_genes_file_name);
    auto piece_strength_normalizer = double(32 + 16 + 2*8 + 2*4 + 2*2 + 8*1);

    auto opponent_pieces_targeted_gene = Opponent_Pieces_Targeted_Gene(&piece_strength_gene);
    auto opponent_pieces_targeted_board = Board("k1K5/8/8/8/8/1rp5/nQb5/1q6 w - - 0 1");
    auto opponent_pieces_targeted_score = (16 + 8 + 4 + 2 + 1)/piece_strength_normalizer;
    tests_passed &= opponent_pieces_targeted_gene.test(opponent_pieces_targeted_board, WHITE, opponent_pieces_targeted_score);

    auto pawn_advancement_gene = Pawn_Advancement_Gene();
    pawn_advancement_gene.read_from(test_genes_file_name);
    auto pawn_advancement_board = Board("7k/4P3/3P4/2P5/1P6/P7/8/K7 w - - 0 1");
    auto pawn_advancement_score = (std::pow(1, 1.2) + std::pow(2, 1.2) + std::pow(3, 1.2) + std::pow(4, 1.2) + std::pow(5, 1.2))/(8*std::pow(5, 1.2));
    tests_passed &= pawn_advancement_gene.test(pawn_advancement_board, WHITE, pawn_advancement_score);

    auto passed_pawn_gene = Passed_Pawn_Gene();
    auto passed_pawn_board = Board("k1K5/8/8/3pP3/3P4/8/8/8 w - - 0 1");
    auto passed_pawn_score = (1.0 + 2.0/3.0)/8;
    tests_passed &= passed_pawn_gene.test(passed_pawn_board, WHITE, passed_pawn_score);

    passed_pawn_board.submit_move(passed_pawn_board.create_move("Kd8"));
    passed_pawn_score = (2.0/3.0)/8;
    tests_passed &= passed_pawn_gene.test(passed_pawn_board, BLACK, passed_pawn_score);

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
    sphere_of_influence_score /= 64*(4.0 + 1.0);
    // Setup       Square score     King distance (from black king)
    // k.......    k4......         k1......
    // ........    .4......         .1......
    // ........    .4......         .2......
    // ........    .4......         .3......
    // .R...p..    4R444411         4R444567
    // ........    .4......         .5......
    // ........    44......         66......
    // K.......    K4......         K7......
    tests_passed &= sphere_of_influence_gene.test(sphere_of_influence_board, WHITE, sphere_of_influence_score);

    auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
    tests_passed &= total_force_gene.test(Board(), WHITE, 1.0);

    auto stacked_pawns_gene = Stacked_Pawns_Gene();
    auto stacked_pawns_board = Board("k7/8/8/8/P7/PP6/PPP5/K7 w - - 0 1");
    tests_passed &= stacked_pawns_gene.test(stacked_pawns_board, WHITE, -3.0/6);

    auto pawn_islands_gene = Pawn_Islands_Gene();
    auto pawn_islands_board = Board("k7/8/8/8/8/8/P1PPP1PP/K7 w - - 0 1");
    tests_passed &= pawn_islands_gene.test(pawn_islands_board, WHITE, (6.0/3)/8);

    auto checkmate_material_gene = Checkmate_Material_Gene();
    auto checkmate_material_board = Board("k7/8/8/8/8/8/8/6RK w - - 0 1");
    tests_passed &= checkmate_material_gene.test(checkmate_material_board, WHITE, 1.0); // white can checkmate
    tests_passed &= checkmate_material_gene.test(checkmate_material_board, BLACK, 0.0); // black cannot



    // String commenting tests
    std::string original = "   a    #     b";
    std::string expected = "a";
    std::string result = String::strip_comments(original, "#");
    if(expected != result)
    {
        std::cerr << "\"" << original << "\" --> \"" << result << "\"" << std::endl;
        std::cerr << "Expected result: \"" << expected << "\"" << std::endl;
        tests_passed = false;
    }

    original = "   a    {    b    }    c   {   d  }   ";
    expected = "a c";
    result = String::strip_block_comment(original, "{", "}");
    if(expected != result)
    {
        std::cerr << "\"" << original << "\" --> \"" << result << "\"" << std::endl;
        std::cerr << "Expected result: \"" << expected << "\"" << std::endl;
        tests_passed = false;
    }

    try
    {
        original = "   a    }    b    {    c   {   d  }   ";
        auto bad_input_result = String::strip_block_comment(original, "{", "}");
        tests_passed = false;
    }
    catch(const std::invalid_argument&)
    {
        // This test should throw an exception.
    }

    try
    {
        original = "   a        b    {    c      d     ";
        auto bad_input_result = String::strip_block_comment(original, "{", "}");
        tests_passed = false;
    }
    catch(const std::invalid_argument&)
    {
        // This test should throw an exception.
    }

    original = "a // b";
    expected = "a";
    result = String::strip_comments(original, "//");
    if(expected != result)
    {
        std::cerr << "Multicharacter comment delimiter test failed." << std::endl;
        std::cerr << "\"" << original << " --> \"" << result << "\"" << std::endl;
        std::cerr << "Expected result: \"" << expected << "\"" << std::endl;
        tests_passed = false;
    }

    original = "a /* b  */ c";
    expected = "a c";
    result = String::strip_block_comment(original, "/*", "*/");
    if(expected != result)
    {
        std::cerr << "Multicharacter block comment delimiter test failed." << std::endl;
        std::cerr << "\"" << original << " --> \"" << result << "\"" << std::endl;
        std::cerr << "Expected result: \"" << expected << "\"" << std::endl;
        tests_passed = false;
    }


    // String::ends_with()/starts_with() tests
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

    std::string end_target = "efg";
    if( ! String::ends_with(search_string, end_target))
    {
        std::cerr << "\"" << end_target << "\" not found at end of " << search_string << std::endl;
        tests_passed = false;
    }

    std::string wrong_end_target = "efh";
    if(String::ends_with(search_string, wrong_end_target))
    {
        std::cerr << "\"" << wrong_end_target << "\" erroneously found at end of " << search_string << std::endl;
        tests_passed = false;
    }

    std::string mixed_case = "AbC dEf";
    std::string lowercase = "abc def";
    auto lowercase_result = String::lowercase(mixed_case);
    if(lowercase_result != lowercase)
    {
        std::cerr << "String::lowercase() returns incorrect results.\n";
        std::cerr << "Original: " << mixed_case << "; Got: " << lowercase_result << "; Expected: " << lowercase << std::endl;
        tests_passed = false;
    }


    // String splitting tests
    std::string splitting_line = "\t a b c d e ";
    std::vector<std::string> expected_split_line = {"a", "b", "c", "d", "e"};
    auto split_line = String::split(splitting_line);
    if(split_line.size() != expected_split_line.size() || ! std::equal(split_line.begin(), split_line.end(), expected_split_line.begin()))
    {
        std::cerr << "These lists should match from line splitting:\nExpected from String::split(" << splitting_line << ")\n";
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

    std::string comma_split_string = ",";
    auto comma_split = String::split(comma_split_string, ",", 1);
    if(comma_split.size() != 2 ||
       ! std::all_of(comma_split.begin(), comma_split.end(), [](const auto& s){ return s.empty(); }))
    {
        std::cerr << "The string \"" << comma_split_string << "\" should split into 2 empty strings." << std::endl;
        tests_passed = false;
    }

    std::string ellipses_split_string = "..a..b..c..d..";
    std::vector<std::string> ellipses_split_expected = {"", "a", "b", "c", "d", ""};
    auto ellipses_split_result = String::split(ellipses_split_string, "..");
    if(ellipses_split_result != ellipses_split_expected)
    {
        tests_passed = false;
        std::cerr << "Incorrect split result:\n";
        std::cerr << ellipses_split_string << " split on .. produced\n";
        for(auto s : ellipses_split_result)
        {
            std::cerr << s << ", ";
        }
        std::cerr << "\ninsead of the expected\n";
        for(auto s : ellipses_split_expected)
        {
            std::cerr << s << ", ";
        }
    }

    auto ellipses_split_count_result = String::split(ellipses_split_string, "..", 4);
    std::vector<std::string> ellipses_split_count_expected = {"", "a", "b", "c", "d.."};
    if(ellipses_split_count_result != ellipses_split_count_expected)
    {
        tests_passed = false;
        std::cerr << "Incorrect split result:\n";
        std::cerr << ellipses_split_string << " split a maximum of 4 times on .. produced\n";
        for(auto s : ellipses_split_count_result)
        {
            std::cerr << s << ", ";
        }
        std::cerr << "\ninsead of the expected\n";
        for(auto s : ellipses_split_count_expected)
        {
            std::cerr << s << ", ";
        }
    }

    auto ellipses_split_count_full_result = String::split(ellipses_split_string, "..", 5);
    if(ellipses_split_count_full_result != ellipses_split_expected)
    {
        tests_passed = false;
        std::cerr << "Incorrect split result:\n";
        std::cerr << ellipses_split_string << " split a maximum of 5 times on .. produced\n";
        for(auto s : ellipses_split_count_full_result)
        {
            std::cerr << s << ", ";
        }
        std::cerr << "\ninsead of the expected\n";
        for(auto s : ellipses_split_expected)
        {
            std::cerr << s << ", ";
        }
    }

    // Number formating
    std::vector<std::pair<int, std::string>> tests =
        {{1, "1"},
         {22, "22"},
         {333, "333"},
         {4444, "4,444"},
         {55555, "55,555"},
         {666666, "666,666"},
         {7777777, "7,777,777"},
         {88888888, "88,888,888"},
         {999999999, "999,999,999"},
         {1000000000, "1,000,000,000"}};
    for(const auto& test : tests)
    {
        auto format_result = String::format_integer(test.first, ",");
        if(format_result != test.second)
        {
            std::cerr << "String::format_integer() failed for "
                      << test.first
                      << ". Got: "
                      << format_result
                      << " instead of "
                      << test.second << std::endl;
            tests_passed = false;
        }
    }


    // Log-Norm distribution check
    const double mean_moves = 26.0;
    const double width = .5;
    const size_t moves_so_far = 22;
    auto moves_left = Math::average_moves_left(mean_moves, width, moves_so_far);
    auto expected_moves_left = 15.2629;
    if(std::abs(moves_left - expected_moves_left) > 1e-4)
    {
        std::cerr << "Log-Norm failed: Expected: " << expected_moves_left
                  << " --- Got: " << moves_left << std::endl;
        tests_passed = false;
    }


    // Clock time reset test
    auto time = 30;
    double expected_time_after_reset = 0.0;
    size_t moves_to_reset = 40;
    auto clock = Clock(time, moves_to_reset);
    clock.start();
    for(size_t i = 0; i < 2*moves_to_reset; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        expected_time_after_reset = clock.time_left(BLACK) + time;
        clock.punch();
    }
    clock.stop();
    if(std::abs(clock.time_left(BLACK) - expected_time_after_reset) > 0.2)
    {
        std::cerr << "Clock reset incorrect: time left for black is " << clock.time_left(BLACK) << " sec. Should be " << expected_time_after_reset << "sec." << std::endl;
        tests_passed = false;
    }

    // Clock time increment test
    auto increment = 5;
    auto clock2 = Clock(time, 0, increment);
    clock2.start();
    double expected_time = time;
    for(size_t i = 0; i < 2*moves_to_reset; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        clock2.punch();
        if(i % 2 == 1) // only on black moves
        {
            expected_time += increment - 0.005;
        }
    }
    clock2.stop();
    if(std::abs(clock2.time_left(BLACK) - expected_time) > 0.2)
    {
        std::cerr << "Clock increment incorrect: time left for black is " << clock2.time_left(BLACK) << " sec. Should be " << expected_time << "sec." << std::endl;
        tests_passed = false;
    }

    // Minimax scoring comparison tests
    Game_Tree_Node_Result r1 = {10,
                                WHITE,
                                {nullptr, nullptr, nullptr}};
    Game_Tree_Node_Result r2 = {10,
                                BLACK,
                                {nullptr, nullptr, nullptr}};

    if(r2.value(WHITE) > r1.value(WHITE))
    {
        std::cerr << "1. Error in comparing Game Tree Node Results." << std::endl;
        tests_passed = false;
    }

    if(r1.value(BLACK) > r2.value(BLACK))
    {
        std::cerr << "2. Error in comparing Game Tree Node Results." << std::endl;
        tests_passed = false;
    }

    Game_Tree_Node_Result alpha_start = {Math::lose_score,
                                         WHITE,
                                         {}};

    Game_Tree_Node_Result beta_start = {Math::win_score,
                                        WHITE,
                                        {}};
    if(alpha_start.value(WHITE) > beta_start.value(WHITE))
    {
        std::cerr << "3. Error in comparing Game Tree Node Results." << std::endl;
        tests_passed = false;
    }

    if(alpha_start.value(BLACK) <= beta_start.value(BLACK))
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
    if(white_win6.value(WHITE) > white_win4.value(WHITE))
    {
        std::cerr << "Later win preferred over earlier win." << std::endl;
        tests_passed = false;
    }

    if(white_win4.value(BLACK) > white_win6.value(BLACK))
    {
        std::cerr << "Earlier loss preferred over later win." << std::endl;
        tests_passed = false;
    }

    Game_Tree_Node_Result black_loss6 = {Math::lose_score,
                                         BLACK,
                                         {nullptr, nullptr, nullptr,
                                          nullptr, nullptr, nullptr,
                                          nullptr}};
    if(white_win6.value(WHITE) != black_loss6.value(WHITE) ||
       white_win6.value(BLACK) != black_loss6.value(BLACK))
    {
        std::cerr << "White win in 6 not equal to black loss in 6." << std::endl;
        tests_passed = false;
    }

    if( ! black_loss6.is_winning_for(WHITE))
    {
        std::cerr << "Black loss in 6 returns false for is_winning_for(WHITE)." << std::endl;
        tests_passed = false;
    }

    if( ! black_loss6.is_losing_for(BLACK))
    {
        std::cerr << "Black loss in 6 returns false for is_losing_for(BLACK)." << std::endl;
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

    return tests_passed;
}

void run_speed_tests()
{
    std::cout << "Gene scoring speed ..." << std::endl;
    auto test_genes_file_name = "testing/test_genome.txt";

    auto castling_possible_gene = Castling_Possible_Gene();
    castling_possible_gene.read_from(test_genes_file_name);
    auto freedom_to_move_gene = Freedom_To_Move_Gene();
    auto king_confinement_gene = King_Confinement_Gene();
    king_confinement_gene.read_from(test_genes_file_name);
    auto king_protection_gene = King_Protection_Gene();
    auto piece_strength_gene = Piece_Strength_Gene();
    piece_strength_gene.read_from(test_genes_file_name);
    auto opponent_pieces_targeted_gene = Opponent_Pieces_Targeted_Gene(&piece_strength_gene);
    auto pawn_advancement_gene = Pawn_Advancement_Gene();
    pawn_advancement_gene.read_from(test_genes_file_name);
    auto passed_pawn_gene = Passed_Pawn_Gene();
    auto sphere_of_influence_gene = Sphere_of_Influence_Gene();
    sphere_of_influence_gene.read_from(test_genes_file_name);
    auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
    auto stacked_pawns_gene = Stacked_Pawns_Gene();
    auto pawn_islands_gene = Pawn_Islands_Gene();
    auto checkmate_material_gene = Checkmate_Material_Gene();
    auto null_gene = Null_Gene();

    auto performance_board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    std::vector<const Gene*> performance_genome = {&castling_possible_gene,
                                                   &checkmate_material_gene,
                                                   &freedom_to_move_gene,
                                                   &king_confinement_gene,
                                                   &king_protection_gene,
                                                   &opponent_pieces_targeted_gene,
                                                   &passed_pawn_gene,
                                                   &pawn_advancement_gene,
                                                   &pawn_islands_gene,
                                                   &sphere_of_influence_gene,
                                                   &stacked_pawns_gene,
                                                   &total_force_gene,
                                                   &null_gene};

    #ifdef NDEBUG
    const auto number_of_tests = 1'000'000;
    const auto time_unit = "us";
    #else
    const auto number_of_tests = 1'000;
    const auto time_unit = "ms";
    #endif // NDEBUG
    std::vector<std::pair<double, std::string>> timing_results;
    auto all_genes_watch = Scoped_Stopwatch("");
    for(auto gene : performance_genome)
    {
        auto score = 0.0;
        auto watch = Scoped_Stopwatch("");
        for(int i = 1; i <= number_of_tests; ++i)
        {
            auto side = performance_board.whose_turn();
            score += gene->evaluate(performance_board, opposite(side), 0);
        }
        timing_results.emplace_back(watch.time_so_far(), gene->name());
    }
    timing_results.emplace_back(all_genes_watch.time_so_far(), "Complete gene scoring");

    std::cout << "Board::submit_move() speed ..." << std::endl;
    auto game_watch = Scoped_Stopwatch("");
    Board speed_board;
    for(auto i = 0; i < number_of_tests; ++i)
    {
        auto move = Random::random_element(speed_board.legal_moves());
        auto move_result = speed_board.submit_move(*move);
        if(move_result.game_has_ended())
        {
            speed_board = Board{};
        }
    }
    timing_results.emplace_back(game_watch.time_so_far(), "Board::submit_move()");

    std::cout << "Board::submit_move() with copy speed ..." << std::endl;
    auto copy_game_watch = Scoped_Stopwatch("");
    Board copy_speed_board;
    for(auto i = 0; i < number_of_tests; ++i)
    {
        auto move = Random::random_element(copy_speed_board.legal_moves());
        auto copy = copy_speed_board;
        auto move_result = copy.submit_move(*move);
        if(move_result.game_has_ended())
        {
            copy_speed_board = Board{};
        }
        else
        {
            copy_speed_board = copy;
        }
    }
    timing_results.emplace_back(copy_game_watch.time_so_far(), "Board::submit_move() with copy");

    std::sort(timing_results.begin(), timing_results.end());
    const auto name_width = std::max_element(timing_results.begin(), timing_results.end(),
                                             [](const auto& x, const auto& y){ return x.second.size() < y.second.size(); })->second.size();
    std::cout << "\n" << std::setw(name_width) << "Test Item" << "   " << "Time (" << time_unit << ")";
    std::cout << "\n" << std::setw(name_width) << "---------" << "   " << "---------" << std::endl;
    for(const auto& result : timing_results)
    {
        std::cout << std::setw(name_width) << result.second << " = " << result.first << std::endl;
    }
}

bool run_perft_tests()
{
    // Count game tree leaves (perft) to given depth to validate move generation
    // (downloaded from http://www.rocechess.ch/perft.html)
    // (leaves from starting positions also found at https://oeis.org/A048987)

    auto perft_suite_input = std::ifstream("testing/perftsuite.epd");
    std::string input_line;
    std::vector<std::string> lines;
    while(std::getline(perft_suite_input, input_line))
    {
        lines.push_back(input_line);
    }
    std::sort(lines.begin(), lines.end(),
              [](auto x, auto y)
              {
                  return std::stoi(String::split(x).back()) < std::stoi(String::split(y).back());
              });

    auto test_number = 0;
    size_t legal_moves_counted = 0;
    auto perft_timer = Scoped_Stopwatch("");
    for(const auto& line : lines)
    {
        auto line_parts = String::split(line, ";");
        auto fen = line_parts.front();
        std::cout << '[' << ++test_number << '/' << lines.size() << "] " << fen << std::flush;
        auto perft_board = Board(fen);
        auto tests = std::vector<std::string>(line_parts.begin() + 1, line_parts.end());
        for(const auto& test : tests)
        {
            auto depth_leaves = String::split(test);
            assert(depth_leaves.size() == 2);
            assert(depth_leaves.front().front() == 'D');
            auto depth = std::stoul(depth_leaves.front().substr(1));
            auto expected_leaves = std::stoul(depth_leaves.back());
            auto leaf_count = move_count(perft_board, depth);
            legal_moves_counted += leaf_count;
            if(leaf_count != expected_leaves)
            {
                std::cerr << "\nError at depth " << depth << std::endl;
                std::cerr << "Expected: " << expected_leaves << ", Got: " << leaf_count << std::endl;
                return false;
            }
            else
            {
                std::cout << '.' << std::flush;
            }
        }

        std::cout << " OK!" << std::endl;
    }

    auto time = perft_timer.time_so_far();
    std::cout << "Perft time: " << time << " seconds" << std::endl;
    std::cout << "Legal moves counted: " << String::format_integer(legal_moves_counted, ",") << std::endl;
    std::cout << "Move generation rate: " << String::format_integer(int(legal_moves_counted/time), ",") << " moves/second." << std::endl;
    return true;
}

void print_randomness_sample()
{
        // Random number generation sample
    int int_width = 10;
    int real_width = 15;
    int norm_width = 15;
    int uint_width = 30;
    std::cout << std::endl;
    std::cout << std::setw(int_width) << "Integers"
              << std::setw(real_width) << "Reals"
              << std::setw(norm_width) << "Laplace"
              << std::setw(uint_width) << "Unsigned Ints" << '\n';
    std::cout << std::setw(int_width) << "+/-1000"
              << std::setw(real_width) << "[0,2]"
              << std::setw(norm_width) << "width = 3"
              << std::setw(uint_width) << "[0," + std::to_string(std::numeric_limits<uint64_t>::max()) << "]\n";
    for(int i = 0; i < 10; ++i)
    {
        std::cout << std::setw(int_width) << Random::random_integer(-1000, 1000)
                  << std::setw(real_width) << Random::random_real(0, 2)
                  << std::setw(norm_width) << Random::random_laplace(3)
                  << std::setw(uint_width) << Random::random_unsigned_int64() << '\n';
    }
}

namespace
{
    bool files_are_identical(const std::string& file_name1, const std::string& file_name2)
    {
        std::ifstream file1(file_name1);
        std::ifstream file2(file_name2);
        int line_count = 0;

        while(file1 && file2)
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
        }

        return ! file1 && ! file2; // both reached end-of-file
    }

    size_t move_count(const Board& board, size_t maximum_depth)
    {
        if(maximum_depth == 0)
        {
            return 1;
        }

        if(maximum_depth == 1)
        {
            return board.legal_moves().size();
        }

        size_t count = 0;
        for(auto move : board.legal_moves())
        {
            auto next_board = board;
            next_board.submit_move(*move);
            count += move_count(next_board, maximum_depth - 1);
        }

        return count;
    }

    bool run_board_tests(const std::string& file_name)
    {
        auto input = std::ifstream(file_name);
        assert(input);
        std::string line;
        auto all_tests_passed = true;

        while(std::getline(input, line))
        {
            line = String::strip_comments(line, "#");
            if(line.empty())
            {
                continue;
            }

            auto test_passed = true;
            auto specification = String::split(line, "|");
            auto test_type = String::lowercase(String::remove_extra_whitespace(specification.at(0)));
            auto board_fen = String::remove_extra_whitespace(specification.at(1));

            try
            {
                auto board = board_fen == "start" ? Board{} : Board(board_fen);
                if(test_type == "invalid board")
                {
                    std::cout << "FAILED" << std::endl;
                    board.ascii_draw(WHITE);
                    std::cout << "This board should not be valid.";
                    test_passed = false;
                }
                else
                {
                    auto moves = String::split(specification.at(2));
                    if(moves.front().empty())
                    {
                        moves.clear();
                    }

                    if(test_type == "all moves legal")
                    {
                        if(!all_moves_legal(board, moves))
                        {
                            test_passed = false;
                        }
                    }
                    else if(test_type == "last move illegal")
                    {
                        assert(moves.size() > 0);
                        auto last_move = moves.back();
                        moves.pop_back();
                        if( ! (all_moves_legal(board, moves) && move_is_illegal(board, last_move)))
                        {
                            test_passed = false;
                        }
                    }
                    else if(test_type == "pinned piece")
                    {
                        assert(moves.size() == 2);
                        auto square = String::remove_extra_whitespace(String::lowercase(moves.front()));
                        assert(moves.front().size() == 2);
                        auto expected_result = String::remove_extra_whitespace(String::lowercase(moves.back()));
                        assert(moves.back() == "true" || moves.back() == "false");
                        if(board.piece_is_pinned(Square(square.front(), square.back() - '0')) != (expected_result == "true"))
                        {
                            std::cout << "FAILED" << std::endl;
                            board.ascii_draw(WHITE);
                            std::cout << "Expected result of " << square << " being pinned: " << expected_result << std::endl;
                            test_passed = false;
                        }
                    }
                    else if(test_type == "move count")
                    {
                        assert(specification.size() == 4);
                        auto expected_count = std::stoi(specification.back());

                        if( ! (all_moves_legal(board, moves) || board.legal_moves().size() != expected_count))
                        {
                            std::cout << "FAILED" << std::endl;
                            board.ascii_draw(WHITE);
                            std::cout << "Legal moves counted: " << board.legal_moves().size() << "; Expected: " << expected_count << std::endl;
                            test_passed = false;
                        }
                    }
                    else if(test_type == "checkmate material")
                    {
                        assert(moves.front() == "true" || moves.front() == "false");
                        auto expected_result = (moves.front() == "true");
                        if(board.enough_material_to_checkmate() != expected_result)
                        {
                            std::cout << "FAILED" << std::endl;
                            board.ascii_draw(WHITE);
                            std::cout << "This board does" << (expected_result ? "" : "not") << " have enough material to checkmate." << std::endl;
                            test_passed = false;
                        }
                    }
                    else if(test_type == "king in check")
                    {
                        assert(specification.size() == 4);
                        auto expected_answer = String::lowercase(String::remove_extra_whitespace(specification.back()));
                        assert(expected_answer == "true" || expected_answer == "false");
                        auto expected_result = expected_answer == "true";
                        if( ! all_moves_legal(board, moves) || board.king_is_in_check() != expected_result)
                        {
                            std::cout << "FAILED" << std::endl;
                            board.ascii_draw(WHITE);
                            std::cout << "King is "
                                      << (expected_result ? "not " : "")
                                      << "in check when it should "
                                      << (expected_result ? "" : "not ")
                                      << "be in check." << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Bad test: " << test_type << std::endl;
                        return false;
                    }
                }
            }
            catch(std::invalid_argument&)
            {
                if(test_type != "invalid board")
                {
                    std::cout << "This should be a valid board." << std::endl;
                    test_passed = false;
                }
            }

            if( ! test_passed)
            {
                std::cout << line << std::endl;
            }
            all_tests_passed = all_tests_passed && test_passed;
        }

        return all_tests_passed;
    }

    bool all_moves_legal(Board& board, const std::vector<std::string>& moves)
    {
        try
        {
            auto game_has_ended = false;
            for(const auto& move : moves)
            {
                if(game_has_ended)
                {
                    throw Illegal_Move("");
                }

                game_has_ended = board.submit_move(board.create_move(move)).game_has_ended();
            }

            return true;
        }
        catch(const Illegal_Move&)
        {
            std::cout << "FAILED" << std::endl;
            board.ascii_draw(WHITE);
            board.print_game_record({}, {}, {}, {}, {});
            std::cerr << "All of these moves should have been legal:";
            for(const auto& move : moves)
            {
                std::cout << " " << move;
            }
            return false;
        }
    }

    bool move_is_illegal(Board& board, const std::string& move)
    {
        try
        {
            board.create_move(move);
            std::cout << "FAILED" << std::endl;
            board.ascii_draw(WHITE);
            std::cerr << "This move should have been illegal: " << move;
            return false;
        }
        catch(const Illegal_Move&)
        {
            return true;
        }
    }
}
