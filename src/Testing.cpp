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
#include <chrono>
using namespace std::chrono_literals;
#include <string>

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

#include "Utility/String.h"
#include "Utility/Scoped_Stopwatch.h"
#include "Utility/Random.h"
#include "Utility/Math.h"

#include "Exceptions/Illegal_Move.h"

namespace
{
    // If expected_result is false, set all_tests_passed to false and print the
    // fail_message to std::cerr. Otherwise, do nothing.
    bool test_result(bool& all_tests_passed, bool expected_result, const std::string& fail_message) noexcept;

    void print() noexcept
    {
        std::cerr << std::endl;
    }

    template<typename Argument_Type>
    void print(const Argument_Type& arg) noexcept
    {
        std::cerr << "'" << arg << "'" << std::endl;
    }

    template<typename First_Argument_Type, typename ...Rest_Argument_Types>
    void print(const First_Argument_Type& first, const Rest_Argument_Types& ... rest) noexcept
    {
        std::cerr << "'" << first << "', ";
        print(rest...);
    }

    // Run the callable f on the arguments. If the result of the argument is not
    // equal to the expecte result, set tests_passed to false and print
    // an error message. Otherwise, do nothing.
    template<typename ...Argument_Types, typename Result_Type, typename Function>
    void test_function(bool& tests_passed, const std::string& test_name, const Result_Type& expected_result, Function f, const Argument_Types& ... arguments) noexcept(noexcept(f))
    {
        auto result = f(arguments...);
        if(result != expected_result)
        {
            std::cerr << test_name << " failed. Expected result: '" << expected_result << "'; Got: '" << result << "'" << std::endl;
            std::cerr << "Arguments: ";
            print(arguments...);
            tests_passed = false;
        }
    }

    // A specialization of the test_function() template that handles
    // functions that return a std::vector<std::string>.
    template<typename ...Argument_Types, typename Function>
    void test_function(bool& tests_passed, const std::string& test_name, const std::vector<std::string>& expected_result, Function f, const Argument_Types& ... arguments)  noexcept(noexcept(f))
    {
        auto result = f(arguments...);
        if(result != expected_result)
        {
            std::cerr << test_name << " failed. Expected result: '";
            for(const auto& s : expected_result)
            {
                std::cerr << s << ", ";
            }
            std::cerr << "'; Got: '";
            for(const auto& s : result)
            {
                std::cerr << s << ", ";
            }
            std::cerr << "'\nArguments: ";
            print(arguments...);
            tests_passed = false;
        }
    }

    template<typename ...Argument_Types, typename Function>
    void function_throw_check(bool& tests_passed, bool should_throw, const std::string& test_name, Function f, const Argument_Types& ... arguments) noexcept
    {
        auto function_threw_exception = false;
        std::string error_message;
        try
        {
            f(arguments...);
        }
        catch(const std::exception& e)
        {
            function_threw_exception = true;
            error_message = e.what();
        }

        if(function_threw_exception != should_throw)
        {
            std::cerr << test_name << " failed. Function should" << (should_throw ? " " : " not ") << "have thrown." << std::endl;
            std::cerr << "Arguments: ";
            print(arguments...);
            if( ! error_message.empty())
            {
                std::cerr << error_message << std::endl;
            }
            tests_passed = false;
        }
    }

    // Equivalent to test_function(), but checks that the callable f does not throw
    // with arguments.
    template<typename ...Argument_Types, typename Function>
    void function_should_not_throw(bool& tests_passed, const std::string& test_name, Function f, const Argument_Types& ... arguments) noexcept
    {
        function_throw_check(tests_passed, false, test_name, f, arguments...);
    }

    // Equivalent to test_function(), but checks that the callable f does throw
    // with arguments.
    template<typename ...Argument_Types, typename Function>
    void function_should_throw(bool& tests_passed, const std::string& test_name, Function f, const Argument_Types& ... arguments) noexcept
    {
        function_throw_check(tests_passed, true, test_name, f, arguments...);
    }

    bool files_are_identical(const std::string& file_name1, const std::string& file_name2) noexcept;
    unsigned long long move_count(const Board& board, unsigned long long maximum_depth) noexcept;
    bool run_board_tests(const std::string& file_name);
    bool all_moves_legal(Board& board, const std::vector<std::string>& moves) noexcept;
    bool move_is_illegal(const Board& board, const std::string& move) noexcept;
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
        test_result(tests_passed, step_index == i, "Direction-index mismatch: " + std::to_string(i) + " --> " + std::to_string(step_index));
    }


    // Square indexing tests
    std::array<bool, 64> visited{};
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto square = Square{file, rank};
            test_result(tests_passed, ! visited[square.index()], "Multiple squares result in same index." + square.string());
            visited[square.index()] = true;
            auto indexed_square = Square(square.file(), square.rank());
            test_result(tests_passed, square == indexed_square, "Incorrect square indexing: " + square.string() + " --> " + indexed_square.string());
        }
    }
    test_result(tests_passed, std::all_of(visited.begin(), visited.end(), [](auto x){ return x; }), "Not all indices visited by iterating through all squares.");


    // Piece construction tests
    for(auto type : {Piece_Type::PAWN, Piece_Type::ROOK, Piece_Type::KNIGHT, Piece_Type::BISHOP, Piece_Type::QUEEN, Piece_Type::KING})
    {
        for(auto color : {Piece_Color::BLACK, Piece_Color::WHITE})
        {
            auto piece = Piece{color, type};
            auto piece2 = Piece{piece.color(), piece.type()};
            test_result(tests_passed, piece == piece2, std::string("Inconsistent construction for ") + piece.fen_symbol() + " --> " + piece2.fen_symbol());
        }
    }


    // Check that square colors are correct
    auto current_color = Square_Color::WHITE;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        current_color = opposite(current_color);
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto square = Square{file, rank};
            test_result(tests_passed, square.color() == current_color, "Wrong color for square " + square.string());
            current_color = opposite(current_color);
        }
    }


    // Check that color equalities are correct
    test_result(tests_passed, Winner_Color::WHITE == static_cast<Winner_Color>(Piece_Color::WHITE), "Winner_Color::WHITE != Piece_Color::WHITE");
    test_result(tests_passed, Winner_Color::BLACK == static_cast<Winner_Color>(Piece_Color::BLACK), "Winner_Color::BLACK != Piece_Color::BLACK");
    test_result(tests_passed, Winner_Color::WHITE != static_cast<Winner_Color>(Piece_Color::BLACK), "Winner_Color::WHITE == Piece_Color::BLACK");
    test_result(tests_passed, Winner_Color::BLACK != static_cast<Winner_Color>(Piece_Color::WHITE), "Winner_Color::BLACK == Piece_Color::WHITE");
    test_result(tests_passed, Winner_Color::NONE != static_cast<Winner_Color>(Piece_Color::WHITE), "Winner_Color::NONE == Piece_Color::WHITE");
    test_result(tests_passed, Winner_Color::NONE != static_cast<Winner_Color>(Piece_Color::BLACK), "Winner_Color::NONE == Piece_Color::BLACK");


    // Check that Square arithmetic works
    for(auto a : Square::all_squares())
    {
        for(auto b : Square::all_squares())
        {
            test_result(tests_passed, a + (b - a) == b,
                "Square arithetic problem: " +
                a.string() + " + (" + b.string() + " - " + a.string() + ") != " + b.string());
        }
    }

    // Check square iteration
    std::array<bool, 64> squares_visited{};
    for(auto square : Square::all_squares())
    {
        test_result(tests_passed, ! squares_visited[square.index()], "Sqaure " + square.string() + " already visited.");
        squares_visited[square.index()] = true;
    }
    test_result(tests_passed, std::all_of(squares_visited.begin(), squares_visited.end(), [](auto tf) { return tf; }), "Square iterator missed some squares.");


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
            repeat_result = repeat_board.submit_move(move);
            if(repeat_result.game_has_ended())
            {
                test_result(tests_passed, repeat_move_count == 8, "Threefold repetition triggered early.");
            }
        }
    }
    test_result(tests_passed, repeat_result.ending_reason() == "Threefold repetition", "Threefold stalemate not triggered.");


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
            if(fifty_move_board_view.piece_on_square(move->start()).type() == Piece_Type::PAWN)
            {
                continue;
            }

            auto next_board = fifty_move_board;
            auto result = next_board.submit_move(*move);

            if(result.winner() != Winner_Color::NONE)
            {
                continue;
            }

            if(result.game_has_ended())
            {
                if(result.ending_reason() == "Threefold repetition")
                {
                    continue;
                }

                test_result(tests_passed, move_counter == 100, "Fifty-move stalemate triggered early.");
            }

            fifty_move_result = fifty_move_board.submit_move(*move);
            move_chosen = true;
            break;
        }

        test_result(tests_passed, move_chosen, "Unable to choose next move (moves made = " + std::to_string(move_counter) + ").");
    }
    test_result(tests_passed, fifty_move_result.ending_reason() == "50-move limit", "50-move draw test result: Got: " + fifty_move_result.ending_reason() + " instead.");

    // Move derivation test
    Board move_derivation_board;
    auto derived_fen = std::string{"rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"};
    auto derived_moves = move_derivation_board.derive_moves(derived_fen);
    for(auto move : derived_moves)
    {
        move_derivation_board.submit_move(*move);
    }
    test_result(tests_passed, move_derivation_board.fen() == derived_fen, "Wrong moves derived.");

    test_result(tests_passed, run_board_tests("testing/board_tests.txt"), "");


    // Board hash with castling tests
    auto castling_hash_board = Board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

    // Lose all castling rights due to king moving
    auto just_kings_move_board = castling_hash_board;
    for(auto move : {"Ke2", "Ke7", "Ke1", "Ke8"})
    {
        just_kings_move_board.submit_move(move);
    }

    // Lose all castling rights due to rooks moving
    auto just_rooks_move_board = castling_hash_board;
    for(auto move : {"Ra2", "Ra7", "Ra1", "Ra8", "Rh2", "Rh7", "Rh1", "Rh8"})
    {
        just_rooks_move_board.submit_move(move);
    }

    test_result(tests_passed, just_kings_move_board.board_hash() == just_rooks_move_board.board_hash(), "Boards should have same hash after castling rights lost");
    test_result(tests_passed, just_kings_move_board.board_hash() != castling_hash_board.board_hash(), "Boards should have different hashes with different castling rights");


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
        ai.mutate(100);
        ai.print(pool_file_name);
    }

    const auto& test_ai = Random::random_element(test_pool);
    test_ai.print(write_file_name);
    auto read_ai = Genetic_AI(pool_file_name, test_ai.id());
    read_ai.print(rewrite_file_name);

    if(test_result(tests_passed, files_are_identical(write_file_name, rewrite_file_name), "Genome loaded from gene pool file not preserved."))
    {
        remove(pool_file_name);
        remove(write_file_name);
        remove(rewrite_file_name);
    }


    // Test Genetic_AI self-assignment
    auto self_write_file_name = "self_original.txt";
    auto self_swap_file_name = "self_swap.txt";
    auto self_assign_file_name = "self_assign.txt";
    remove(self_write_file_name);
    remove(self_swap_file_name);
    remove(self_assign_file_name);

    read_ai.print(self_write_file_name);

    std::swap(read_ai, read_ai);
    read_ai.print(self_swap_file_name);
    auto self_swap_passed = test_result(tests_passed, files_are_identical(self_write_file_name, self_swap_file_name), "Self-swap changed AI.");

    const auto& copy_ai = read_ai;
    read_ai = copy_ai;
    read_ai.print(self_assign_file_name);
    auto self_assign_passed = test_result(tests_passed, files_are_identical(self_write_file_name, self_assign_file_name), "Self-assignment changed AI.");

    if(self_swap_passed && self_assign_passed)
    {
        remove(self_write_file_name);
        remove(self_swap_file_name);
        remove(self_assign_file_name);
    }

    auto file_name = "genetic_ai_example.txt";
    std::string line;
    std::getline(std::ifstream(file_name), line);
    auto id = std::stoi(String::split(line).at(1));
    function_should_not_throw(tests_passed, "Genetic_AI ctor", [=]() { Genetic_AI{file_name, id}; });


    // Test individual board-scoring genes
    auto test_genes_file_name = "testing/test_genome.txt";

    auto castling_possible_gene = Castling_Possible_Gene();
    castling_possible_gene.read_from(test_genes_file_name);
    auto castling_board = Board("rn2k3/8/8/8/8/8/8/R3K2R w KQq - 0 1");
    auto white_castling_score = 0.8*(5.0/6.0) + 0.2*(6.0/7.0); // maximum score with and without actually castling
    castling_possible_gene.test(tests_passed, castling_board, Piece_Color::WHITE, white_castling_score);

    castling_board.submit_move("O-O");
    castling_possible_gene.test(tests_passed, castling_board, Piece_Color::WHITE, 1.0); // full score for kingside castling

    auto black_castling_score = 0.2*(5.0/7.0); // castling possible
    castling_possible_gene.test(tests_passed, castling_board, Piece_Color::BLACK, black_castling_score);

    castling_board.submit_move("Nc6");
    castling_possible_gene.test(tests_passed, castling_board, Piece_Color::WHITE, 0.0); // castling no longer relevant

    auto freedom_to_move_gene = Freedom_To_Move_Gene();
    auto freedom_to_move_board = Board("5k2/8/8/8/4Q3/8/8/3K4 w - - 0 1");
    auto freedom_to_move_white_score = 32.0/20.0;
    freedom_to_move_gene.test(tests_passed, freedom_to_move_board, Piece_Color::WHITE, freedom_to_move_white_score);
    freedom_to_move_board.submit_move("Qd5");
    auto freedom_to_move_black_score = 3.0/20.0;
    freedom_to_move_gene.test(tests_passed, freedom_to_move_board, Piece_Color::BLACK, freedom_to_move_black_score);
    freedom_to_move_gene.test(tests_passed, freedom_to_move_board, Piece_Color::WHITE, freedom_to_move_white_score);

    auto king_confinement_gene = King_Confinement_Gene();
    king_confinement_gene.read_from(test_genes_file_name);
    auto king_confinement_board = Board("k3r3/8/8/8/8/8/5PPP/7K w - - 0 1");
    auto king_confinement_score = (4*(1.0 + 1.0 + 1.0) + // blocked by friendlies (h2, g2, f2)
                                   (-1)*(1.0 + 1.0))/ // blocked by opponent (e1, e2)
                                   (4 + 1)/ // normalizing
                                   (1 + (1 + 1 + 1)); // free squares (h1, g1, f1)
    king_confinement_gene.test(tests_passed, king_confinement_board, Piece_Color::WHITE, king_confinement_score);

    auto king_confined_by_pawns_board = Board("k7/8/8/8/8/pppppppp/8/K7 w - - 0 1");
    auto king_confined_by_pawns_score = (4*(0.0) + // no friendly blockers
                                         (-1)*(8.0))/ // blocked by pawn attacks on second rank
                                         (4 + 1)/ // normalizing
                                         (1 + 8); // free squares (a1-h1)
    king_confinement_gene.test(tests_passed, king_confined_by_pawns_board, Piece_Color::WHITE, king_confined_by_pawns_score);

    auto king_protection_gene = King_Protection_Gene();
    auto king_protection_board = king_confinement_board;
    auto max_square_count = 8 + 7 + 7 + 7 + 6; // max_square_count in King_Protection_Gene.cpp
    auto square_count = 7 + 1; // row attack along rank 1 + knight attack from g3
    auto king_protection_score = double(max_square_count - square_count)/max_square_count;
    king_protection_gene.test(tests_passed, king_protection_board, Piece_Color::WHITE, king_protection_score);

    auto piece_strength_gene = Piece_Strength_Gene();
    piece_strength_gene.read_from(test_genes_file_name);
    auto piece_strength_normalizer = double(32 + 16 + 2*8 + 2*4 + 2*2 + 8*1);

    auto opponent_pieces_targeted_gene = Opponent_Pieces_Targeted_Gene(&piece_strength_gene);
    auto opponent_pieces_targeted_board = Board("k1K5/8/8/8/8/1rp5/nQb5/1q6 w - - 0 1");
    auto opponent_pieces_targeted_score = (16 + 8 + 4 + 2 + 1)/piece_strength_normalizer;
    opponent_pieces_targeted_gene.test(tests_passed, opponent_pieces_targeted_board, Piece_Color::WHITE, opponent_pieces_targeted_score);

    auto pawn_advancement_gene = Pawn_Advancement_Gene();
    auto pawn_advancement_board = Board("7k/4P3/3P4/2P5/1P6/P7/8/K7 w - - 0 1");
    auto pawn_advancement_score = double(1 + 2 + 3 + 4 + 5)/(8*5);
    pawn_advancement_gene.test(tests_passed, pawn_advancement_board, Piece_Color::WHITE, pawn_advancement_score);

    auto passed_pawn_gene = Passed_Pawn_Gene();
    auto passed_pawn_board = Board("k1K5/8/8/3pP3/3P4/8/8/8 w - - 0 1");
    auto passed_pawn_score = (1.0 + 2.0/3.0)/8;
    passed_pawn_gene.test(tests_passed, passed_pawn_board, Piece_Color::WHITE, passed_pawn_score);

    passed_pawn_board.submit_move("Kd8");
    passed_pawn_score = (2.0/3.0)/8;
    passed_pawn_gene.test(tests_passed, passed_pawn_board, Piece_Color::BLACK, passed_pawn_score);

    auto sphere_of_influence_gene = Sphere_of_Influence_Gene();
    sphere_of_influence_gene.read_from(test_genes_file_name);
    auto sphere_of_influence_board = Board("k7/8/8/8/1R3p2/8/8/K7 w - - 0 1");
    auto sphere_of_influence_score
        = (4.0*(1 + (2.0/(1 + 1.0))))  // b8
        + (4.0*(1 + (2.0/(1 + 1.0))))  // b7
        + (4.0*(1 + (2.0/(1 + 2.0))))  // b6
        + (4.0*(1 + (2.0/(1 + 3.0))))  // b5
        + (4.0*(1 + (2.0/(1 + 5.0))))  // b3
        + (4.0*(1 + (2.0/(1 + 6.0))))  // b2
        + (4.0*(1 + (2.0/(1 + 7.0))))  // b1
        + (4.0*(1 + (2.0/(1 + 4.0))))  // a4
        + (4.0*(1 + (2.0/(1 + 6.0))))  // a2
        + (4.0*(1 + (2.0/(1 + 4.0))))  // c4
        + (4.0*(1 + (2.0/(1 + 4.0))))  // d4
        + (4.0*(1 + (2.0/(1 + 4.0))))  // e4
        + (4.0*(1 + (2.0/(1 + 5.0))))  // f4
        + (1.0*(1 + (2.0/(1 + 6.0))))  // g4
        + (1.0*(1 + (2.0/(1 + 7.0)))); // h4
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
    sphere_of_influence_gene.test(tests_passed, sphere_of_influence_board, Piece_Color::WHITE, sphere_of_influence_score);

    auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
    total_force_gene.test(tests_passed, Board(), Piece_Color::WHITE, 1.0);

    auto stacked_pawns_gene = Stacked_Pawns_Gene();
    auto stacked_pawns_board = Board("k7/8/8/8/P7/PP6/PPP5/K7 w - - 0 1");
    stacked_pawns_gene.test(tests_passed, stacked_pawns_board, Piece_Color::WHITE, -3.0/6);

    auto pawn_islands_gene = Pawn_Islands_Gene();
    auto pawn_islands_board = Board("k7/8/8/8/8/8/P1PPP1PP/K7 w - - 0 1");
    pawn_islands_gene.test(tests_passed, pawn_islands_board, Piece_Color::WHITE, (6.0/3)/8);

    auto checkmate_material_gene = Checkmate_Material_Gene();
    auto checkmate_material_board = Board("k7/8/8/8/8/8/8/6RK w - - 0 1");
    checkmate_material_gene.test(tests_passed, checkmate_material_board, Piece_Color::WHITE, 1.0); // white can checkmate
    checkmate_material_gene.test(tests_passed, checkmate_material_board, Piece_Color::BLACK, 0.0); // black cannot

    test_function(tests_passed, "Strip single-character comments", "a", String::strip_comments, "   a    #     b", "#");
    test_function(tests_passed, "Strip block comments", "a c", String::strip_block_comment, "   a    {    b    }    c   {   d  }   ", "{", "}");

    function_should_throw(tests_passed, "Bad block comments", String::strip_block_comment, "   a    }    b    {    c   {   d  }   ", "{", "}");
    function_should_throw(tests_passed, "Bad block comments", String::strip_block_comment, "   a        b    {    c      d     ", "{", "}");

    test_function(tests_passed, "Delimited text extraction", "a(b", String::extract_delimited_text, "(a(b))", "(", ")");

    test_function(tests_passed, "Nested block deletion", "ae", String::strip_nested_block_comments, "a(b(c)d)e", "(", ")");
    function_should_throw(tests_passed, "Invalid nested block delimiters", String::strip_nested_block_comments, "", "??", "???");

    test_function(tests_passed, "Strip multicharacter comment", "a", String::strip_comments, "a // b", "//");
    test_function(tests_passed, "Multicharacter block comment", "a c", String::strip_block_comment, "a /* b  */ c", "/*", "*/");
    test_function(tests_passed, "String::starts_with()", true, String::starts_with, "abcdefg", "abc");
    test_function(tests_passed, "String::starts_with()", false, String::starts_with, "abcdefg", "abd");
    test_function(tests_passed, "String::ends_with()", true, String::ends_with, "abcdefg", "efg");
    test_function(tests_passed, "String::ends_with()", false, String::ends_with, "abcdefg", "efh");
    test_function(tests_passed, "String::lowercase()", "abc def", String::lowercase, "AbC dEf");
    test_function(tests_passed, "String::add_to_file_name() with dot", "a-b.c", String::add_to_file_name, "a.c", "-b");
    test_function(tests_passed, "String::add_to_file_name() with no dot", "a-b", String::add_to_file_name, "a", "-b");

    test_function(tests_passed, "String::split()", {"a", "b", "c", "d", "e"}, String::split, "\t a b c d e ", "", -1);
    test_function(tests_passed, "Split on comma", {"", ""}, String::split, ",", ",", 1);
    test_function(tests_passed, "Ellipses split", {"", "a", "b", "c", "d", ""}, String::split, "..a..b..c..d..", "..", -1);
    test_function(tests_passed, "Ellipses split", {"", "a", "b", "c", "d.."}, String::split, "..a..b..c..d..", "..", 4);
    test_function(tests_passed, "Ellipses split", {"", "a", "b", "c", "d", ""}, String::split, "..a..b..c..d..", "..", 5);

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
    for(const auto& [number, text] : tests)
    {
        test_function(tests_passed, "Format integer (size_t)", text, String::format_integer<size_t>, number, ",");
        test_function(tests_passed, "Format integer (int)", text, String::format_integer<int>, number, ",");
        test_function(tests_passed, "Format integer (negative int)", "-" + text, String::format_integer<int>, -number, ",");
    }

    // String to size_t number conversion
    function_should_not_throw(tests_passed, "Non-throwing string to size_t", String::string_to_number<size_t>, "123");
    function_should_not_throw(tests_passed, "Non-throwing string to size_t with whitespace", String::string_to_number<size_t>, " 456  ");
    function_should_throw(tests_passed, "Throwing string to size_t", String::string_to_number<size_t>, "78x9");


    // Log-Norm distribution check
    const double mean_moves = 26.0;
    const double width = .5;
    const size_t moves_so_far = 22;
    auto moves_left = Math::average_moves_left(mean_moves, width, moves_so_far);
    auto expected_moves_left = 15.2629;
    test_result(tests_passed, std::abs(moves_left - expected_moves_left) < 1e-4,
                std::string("Log-Norm failed: Expected: ") + std::to_string(expected_moves_left) +
                " --- Got: " + std::to_string(moves_left));


    // Clock time reset test
    auto time = 30;
    double expected_time_after_reset = 0.0;
    size_t moves_to_reset = 40;
    Board timing_board;
    auto clock = Clock(time, moves_to_reset);
    clock.start();
    for(size_t i = 0; i < 2*moves_to_reset; ++i)
    {
        std::this_thread::sleep_for(5ms);
        expected_time_after_reset = clock.time_left(Piece_Color::BLACK) + time;
        clock.punch(timing_board);
    }
    clock.stop();
    test_result(tests_passed, std::abs(clock.time_left(Piece_Color::BLACK) - expected_time_after_reset) < 0.2,
                "Clock reset incorrect: time left for black is " + std::to_string(clock.time_left(Piece_Color::BLACK)) + " sec." +
                " Should be " + std::to_string(expected_time_after_reset) + "sec.");

    // Clock time increment test
    auto increment = 5;
    auto clock2 = Clock(time, 0, increment);
    clock2.start();
    double expected_time = time;
    for(size_t i = 0; i < 2*moves_to_reset; ++i)
    {
        std::this_thread::sleep_for(5ms);
        clock2.punch(timing_board);
        if(i % 2 == 1) // only on black moves
        {
            expected_time += increment - 0.005;
        }
    }
    clock2.stop();
    test_result(tests_passed, std::abs(clock2.time_left(Piece_Color::BLACK) - expected_time) < 0.2,
                std::string("Clock increment incorrect: time left for black is ") + std::to_string(clock2.time_left(Piece_Color::BLACK)) + " sec." +
                " Should be " + std::to_string(expected_time) + "sec.");


    // Minimax scoring comparison tests
    Game_Tree_Node_Result r1 = {10,
                                Piece_Color::WHITE,
                                {nullptr, nullptr, nullptr}};
    Game_Tree_Node_Result r2 = {10,
                                Piece_Color::BLACK,
                                {nullptr, nullptr, nullptr}};

    test_result(tests_passed, r2.value(Piece_Color::WHITE) < r1.value(Piece_Color::WHITE), "1. Error in comparing Game Tree Node Results.");
    test_result(tests_passed, r1.value(Piece_Color::BLACK) < r2.value(Piece_Color::BLACK), "2. Error in comparing Game Tree Node Results.");

    Game_Tree_Node_Result alpha_start = {Game_Tree_Node_Result::lose_score,
                                         Piece_Color::WHITE,
                                         {}};

    Game_Tree_Node_Result beta_start = {Game_Tree_Node_Result::win_score,
                                        Piece_Color::WHITE,
                                        {}};
    test_result(tests_passed, alpha_start.value(Piece_Color::WHITE) < beta_start.value(Piece_Color::WHITE), "3. Error in comparing Game Tree Node Results.");
    test_result(tests_passed, alpha_start.value(Piece_Color::BLACK) > beta_start.value(Piece_Color::BLACK), "4. Error in comparing Game Tree Node Results.");

    Game_Tree_Node_Result white_win4 = {Game_Tree_Node_Result::win_score,
                                        Piece_Color::WHITE,
                                        {nullptr, nullptr, nullptr,
                                         nullptr, nullptr}};
    Game_Tree_Node_Result white_win6 = {Game_Tree_Node_Result::win_score,
                                        Piece_Color::WHITE,
                                        {nullptr, nullptr, nullptr,
                                         nullptr, nullptr, nullptr,
                                         nullptr}};
    test_result(tests_passed, white_win6.value(Piece_Color::WHITE) < white_win4.value(Piece_Color::WHITE), "Later win preferred over earlier win.");
    test_result(tests_passed, white_win4.value(Piece_Color::BLACK) < white_win6.value(Piece_Color::BLACK), "Earlier loss preferred over later win.");

    Game_Tree_Node_Result black_loss6 = {Game_Tree_Node_Result::lose_score,
                                         Piece_Color::BLACK,
                                         {nullptr, nullptr, nullptr,
                                          nullptr, nullptr, nullptr,
                                          nullptr}};
    test_result(tests_passed, white_win6.value(Piece_Color::WHITE) == black_loss6.value(Piece_Color::WHITE), "1. White win in 6 not equal to black loss in 6.");
    test_result(tests_passed, white_win6.value(Piece_Color::BLACK) == black_loss6.value(Piece_Color::BLACK), "2. White win in 6 not equal to black loss in 6.");

    test_result(tests_passed, black_loss6.is_winning_for(Piece_Color::WHITE), "Black loss in 6 returns false for is_winning_for(Piece_Color::WHITE).");
    test_result(tests_passed, black_loss6.is_losing_for(Piece_Color::BLACK), "Black loss in 6 returns false for is_losing_for(Piece_Color::BLACK).");


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
    auto passed_pawn_gene = Passed_Pawn_Gene();
    auto sphere_of_influence_gene = Sphere_of_Influence_Gene();
    sphere_of_influence_gene.read_from(test_genes_file_name);
    auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
    auto stacked_pawns_gene = Stacked_Pawns_Gene();
    auto pawn_islands_gene = Pawn_Islands_Gene();
    auto checkmate_material_gene = Checkmate_Material_Gene();

    auto performance_board = Board();
    for(const auto& move : String::split("e4 e6 d4 h5 d5 b5 Qf3 g6 Be2 Bg7 Bd2 h4 Nh3 Na6 Nc3 b4 Nf4 Nc5 Nd3 Na4 Ne5 Nb6 Qd3 Qe7 Qe3 Ba6 Qf3 Nf6 Qe3 h3 Qf4 Qc5 Qf3 Qe7"))
    {
        performance_board.submit_move(move);
    }

    if(performance_board.fen() != "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 4 18")
    {
        throw std::logic_error("Different FEN from previous tests.");
    }

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
                                                   &total_force_gene};

    #ifdef NDEBUG
    const auto number_of_tests = 1'000'000;
    const auto time_unit = "us";
    #else
    const auto number_of_tests = 1'000;
    const auto time_unit = "ms";
    #endif // NDEBUG
    std::vector<std::pair<double, std::string>> timing_results;
    auto all_genes_watch = Scoped_Stopwatch::start_stopwatch("");
    for(auto gene : performance_genome)
    {
        auto score = 0.0;
        auto watch = Scoped_Stopwatch::start_stopwatch("");
        for(int i = 1; i <= number_of_tests; ++i)
        {
            auto side = performance_board.whose_turn();
            score += gene->evaluate(performance_board, opposite(side), performance_board.game_length());
        }
        timing_results.emplace_back(watch.time_so_far(), gene->name());
    }
    timing_results.emplace_back(all_genes_watch.time_so_far(), "Complete gene scoring");

    std::cout << "Board::submit_move() speed ..." << std::endl;
    auto game_watch = Scoped_Stopwatch::start_stopwatch("");
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
    auto board_submit_time = timing_results.back().first;

    std::cout << "Board::submit_move() with copy speed ..." << std::endl;
    auto copy_game_watch = Scoped_Stopwatch::start_stopwatch("");
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

    std::cout << "Board::quiescent() speed ... " << std::flush;
    auto quiescent_watch = Scoped_Stopwatch::start_stopwatch("");
    Board quiescent_board;
    size_t move_count = 0;
    for(auto i = 0; i < number_of_tests; ++i)
    {
        while(true)
        {
            auto move = Random::random_element(quiescent_board.legal_moves());
            auto move_result = quiescent_board.submit_move(*move);
            ++move_count;
            if(move_result.game_has_ended())
            {
                quiescent_board = Board{};
            }
            else if( ! quiescent_board.safe_for_king(move->end(), opposite(quiescent_board.whose_turn())))
            {
                break;
            }
        }

        auto quiescent_result_board = quiescent_board.quiescent({1.0, 5.0, 3.0, 3.0, 8.0, 100.0});
    }
    timing_results.emplace_back(quiescent_watch.time_so_far() - (board_submit_time*move_count)/number_of_tests, "Board::quiescent()");
    std::cout << "(non-quiescent moves = " << String::format_integer(move_count, ",") << ")" << std::endl;

    std::sort(timing_results.begin(), timing_results.end());
    const auto name_width = std::max_element(timing_results.begin(), timing_results.end(),
                                             [](const auto& x, const auto& y){ return x.second.size() < y.second.size(); })->second.size();
    std::cout << "\n" << std::setw(name_width) << "Test Item" << "   " << "Time (" << time_unit << ")";
    std::cout << "\n" << std::setw(name_width) << "---------" << "   " << "---------" << std::endl;
    for(const auto& [time, name] : timing_results)
    {
        std::cout << std::setw(name_width) << name << " = " << time << std::endl;
    }
}

bool run_perft_tests()
{
    // Count game tree leaves (perft) to given depth to validate move generation
    // (downloaded from http://www.rocechess.ch/perft.html)
    // (leaves from starting positions also found at https://oeis.org/A048987)

    std::vector<int> tests_failed;
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
                  return std::stoull(String::split(x).back()) < std::stoull(String::split(y).back());
              });

    auto test_number = 0;
    unsigned long long legal_moves_counted = 0;
    auto perft_timer = Scoped_Stopwatch::start_stopwatch("");
    for(const auto& line : lines)
    {
        auto time_at_start = perft_timer.time_so_far();
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
            auto depth = std::stoull(depth_leaves.front().substr(1));
            auto expected_leaves = std::stoull(depth_leaves.back());
            auto leaf_count = move_count(perft_board, depth);
            legal_moves_counted += leaf_count;
            if(leaf_count != expected_leaves)
            {
                std::cout << 'x';
                tests_failed.push_back(test_number);
                break;
            }
            else
            {
                std::cout << '.' << std::flush;
            }
        }

        std::cout << " ";
        if(tests_failed.empty() || tests_failed.back() != test_number)
        {
            std::cout << "OK! ";
        }
        auto time_at_end = perft_timer.time_so_far();
        std::cout << time_at_end - time_at_start << " / " << time_at_end << std::endl;
    }

    auto time = perft_timer.time_so_far();
    std::cout << "Perft time: " << time << " seconds" << std::endl;
    std::cout << "Legal moves counted: " << String::format_integer(int(legal_moves_counted), ",") << std::endl;
    std::cout << "Move generation rate: " << String::format_integer(int(legal_moves_counted/time), ",") << " moves/second." << std::endl;
    if( ! tests_failed.empty())
    {
        std::cout << "Test" << (tests_failed.size() > 1 ? "s" : "") << " failed: ";
        for(auto t : tests_failed)
        {
            std::cout << t << " ";
        }
        std::cout << std::endl;
    }
    return tests_failed.empty();
}

void print_randomness_sample()
{
    // Random number generation sample
    int int_width = 10;
    int real_width = 15;
    int norm_width = 15;
    int uint_width = 30;
    std::cout << std::setw(int_width) << "Integers"
              << std::setw(real_width) << "Reals"
              << std::setw(norm_width) << "Laplace"
              << std::setw(uint_width) << "Unsigned Ints" << '\n';
    std::cout << std::setw(int_width) << "+/-1000"
              << std::setw(real_width) << "[0,2]"
              << std::setw(norm_width) << "width = 3"
              << std::setw(uint_width) << "[0," + std::to_string(std::numeric_limits<uint64_t>::max()) + "]" << "\n";
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
    bool test_result(bool& all_tests_passed, bool expected_result, const std::string& fail_message) noexcept
    {
        if( ! expected_result)
        {
            all_tests_passed = false;
            std::cerr << fail_message << std::endl;
        }

        return expected_result;
    }

    bool files_are_identical(const std::string& file_name1, const std::string& file_name2) noexcept
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
                if(String::contains(line1, ':') && String::contains(line2, ':'))
                {
                    auto split1 = String::split(line1, ":", 1);
                    auto split2 = String::split(line2, ":", 1);
                    if(split1.front() == split2.front())
                    {
                        auto num1 = std::stod(split1.back());
                        auto num2 = std::stod(split2.back());
                        if(std::abs((num1 - num2)/(num1 + num2)) < 1e-6)
                        {
                            continue;
                        }
                    }
                }

                std::cerr << "Mismatch at line " << line_count << ":\n";
                std::cerr << line1 << " != " << line2 << "\n";
                return false;
            }
        }

        if(file1)
        {
            std::cerr << file_name1 << " is longer than " << file_name2 << std::endl;
            return false;
        }

        if(file2)
        {
            std::cerr << file_name2 << " is longer than " << file_name1 << std::endl;
            return false;
        }

        return true;
    }

    unsigned long long move_count(const Board& board, unsigned long long maximum_depth) noexcept
    {
        if(maximum_depth == 0)
        {
            return 1;
        }

        if(maximum_depth == 1)
        {
            return board.legal_moves().size();
        }

        unsigned long long count = 0;
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

            auto specification = String::split(line, "|");
            assert(specification.size() >= 3);

            auto test_type = String::lowercase(String::remove_extra_whitespace(specification.at(0)));
            auto board_fen = String::remove_extra_whitespace(specification.at(1));
            auto board = board_fen == "start" ? Board{} : Board{board_fen};
            auto test_passed = true;

            if(test_type == "all moves legal")
            {
                auto moves = String::split(specification.at(2));
                test_result(test_passed, all_moves_legal(board, moves), "");
            }
            else if(test_type == "last move illegal")
            {
                auto moves = String::split(specification.at(2));
                assert(moves.size() > 0);
                auto last_move = moves.back();
                moves.pop_back();
                test_result(test_passed, all_moves_legal(board, moves) && move_is_illegal(board, last_move), "");
            }
            else if(test_type == "pinned piece")
            {
                auto square_result = String::split(specification.at(2));
                assert(square_result.size() == 2);
                assert(square_result.front().size() == 2);

                auto square = String::remove_extra_whitespace(String::lowercase(square_result.front()));
                auto expected_result = String::remove_extra_whitespace(String::lowercase(square_result.back()));

                assert(expected_result == "true" || expected_result == "false");
                auto expected_bool = (expected_result == "true");
                test_result(test_passed, board.piece_is_pinned(Square(square.front(), square.back() - '0')) == expected_bool,
                            "Expected result of " + square + " being pinned: " + expected_result);
            }
            else if(test_type == "move count")
            {
                assert(specification.size() == 4);
                auto moves = String::split(specification.at(2));
                auto expected_count = String::string_to_number<size_t>(specification.back());
                test_result(test_passed, all_moves_legal(board, moves) && board.legal_moves().size() == expected_count,
                            "Legal moves counted: " + std::to_string(board.legal_moves().size()) + "; Expected: " + std::to_string(expected_count));
            }
            else if(test_type == "checkmate material")
            {
                auto result_text = String::remove_extra_whitespace(specification.back());
                assert(result_text == "true" || result_text == "false");
                auto expected_result = (result_text == "true");
                test_result(test_passed, board.enough_material_to_checkmate() == expected_result,
                            std::string("This board does") + (expected_result ? "" : " not") + " have enough material to checkmate.");
            }
            else if(test_type == "king in check")
            {
                assert(specification.size() == 4);
                auto moves = String::split(specification.at(2));
                auto expected_answer = String::lowercase(String::remove_extra_whitespace(specification.back()));
                assert(expected_answer == "true" || expected_answer == "false");
                auto expected_result = expected_answer == "true";
                test_result(test_passed, all_moves_legal(board, moves) && board.king_is_in_check() == expected_result,
                            std::string("King is ") +
                                (expected_result ? "not " : "") +
                                "in check when it should " +
                                (expected_result ? "" : "not ") +
                                "be in check.");
            }
            else if(test_type == "quiescent")
            {
                assert(specification.size() == 4);
                auto moves = String::split(specification.at(2));
                test_result(test_passed, all_moves_legal(board, moves), "Bad test: Illegal moves");
                auto actual_result_board = board;
                for(auto move : board.quiescent({1.0, 5.0, 3.0, 3.0, 8.0, 100.0}))
                {
                    actual_result_board.submit_move(*move);
                }
                for(auto quiescent_move : String::split(specification.at(3)))
                {
                    board.submit_move(quiescent_move);
                }
                test_result(test_passed,
                            board.fen() == actual_result_board.fen(),
                            "Expected: " + board.fen() + "\nGot:      " + actual_result_board.fen());
            }
            else
            {
                test_result(test_passed, false, "Bad test: " + test_type);
            }

            test_result(all_tests_passed, test_passed, line + " -- FAILED\n");
        }

        return all_tests_passed;
    }

    bool all_moves_legal(Board& board, const std::vector<std::string>& moves) noexcept
    {
        bool result = true;
        auto game_has_ended = false;
        for(const auto& move : moves)
        {
            function_should_not_throw(result, move + " should be legal",
                [&]()
                {
                    if(game_has_ended)
                    {
                        throw Illegal_Move("");
                    }

                    game_has_ended = board.submit_move(move).game_has_ended();
                });
        }

        return result;
    }

    bool move_is_illegal(const Board& board, const std::string& move) noexcept
    {
        bool result = true;
        function_should_throw(result, move + " should be illegal", [&](){ board.create_move(move); });
        return result;
    }
}
