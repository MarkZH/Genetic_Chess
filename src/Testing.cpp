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

#include "Players/Minimax_AI.h"
#include "Players/Game_Tree_Node_Result.h"
#include "Players/Alpha_Beta_Value.h"

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
#include "Genes/Pawn_Structure_Gene.h"

#include "Utility/String.h"
#include "Utility/Random.h"
#include "Utility/Math.h"
#include "Utility/Exceptions.h"
#include "Utility/Algorithm.h"

namespace
{
    // If expected_result is false, set all_tests_passed to false and print the
    // fail_message to std::cerr. Otherwise, do nothing.
    bool test_result(bool& all_tests_passed, const bool expected_result, const std::string& fail_message) noexcept;


    void print_argument_leader()
    {
        std::cerr << "Argument: (";
    }

    void print_argument_trailer()
    {
        std::cerr << ")" << std::endl;
    }

    void print_arguments() noexcept
    {
        print_argument_leader();
        print_argument_trailer();
    }

    template<typename Argument_Type>
    void print_list(const Argument_Type& arg) noexcept
    {
        std::cerr << "'" << arg << "'";
    }

    template<typename First_Argument_Type, typename ...Rest_Argument_Types>
    void print_list(const First_Argument_Type& first, const Rest_Argument_Types& ... rest) noexcept
    {
        print_list(first);
        std::cerr << ", ";
        print_list(rest ...);
    }
    template<typename ...Argument_Types>
    void print_arguments(const Argument_Types& ... arguments) noexcept
    {
        print_argument_leader();
        print_list(arguments...);
        print_argument_trailer();
    }

    template<typename Result_Type>
    void print_result(const Result_Type& result)
    {
        std::cerr << result;
    }

    template<>
    void print_result(const std::vector<std::string>& results)
    {
        std::cerr << "{" << String::join(results.begin(), results.end(), ", ") << "}";
    }

    // Run the callable f on the arguments. If the result of the argument is not
    // equal to the expected result, set tests_passed to false and print
    // an error message. Otherwise, do nothing.
    template<typename ...Argument_Types, typename Result_Type, typename Function>
    void test_function(bool& tests_passed, const std::string& test_name, const Result_Type& expected_result, Function f, const Argument_Types& ... arguments) noexcept(noexcept(f))
    {
        const auto result = f(arguments...);
        if(result != expected_result)
        {
            std::cerr << test_name << " failed. Expected result: '";
            print_result(expected_result);
            std::cerr << "'; Got: '";
            print_result(result);
            std::cerr << "'" << std::endl;
            print_arguments(arguments...);
            tests_passed = false;
        }
    }

    // Run callable f on the arguments. If the function throws, set tests_passed to false and print
    // an error message. Otherwise, do nothing.
    template<typename ...Argument_Types, typename Function>
    void function_should_not_throw(bool& tests_passed, const std::string& test_name, Function f, const Argument_Types& ... arguments) noexcept
    {
        try
        {
            f(arguments...);
        }
        catch(const std::exception& e)
        {
            std::cerr << (test_name.empty() ? "Test" : test_name) << " failed. Function should not have thrown." << std::endl;
            std::cerr << e.what() << std::endl;
            print_arguments(arguments...);
            tests_passed = false;
        }
    }

    // Run callable f on the arguments, if the function does not throw or throws the wrong exception type,
    // set tests_passed to false and print an error message. Otherwise, do nothing.
    template<typename Error, typename ...Argument_Types, typename Function>
    void function_should_throw(bool& tests_passed, const std::string& test_name, Function f, const Argument_Types& ... arguments) noexcept
    {
        const auto test_title = (test_name.empty() ? "Test" : test_name);

        try
        {
            f(arguments...);

            std::cerr << test_title << " failed. Function should have thrown but did not.\n";
        }
        catch(const Error&)
        {
            return;
        }
        catch(const std::exception& e)
        {
            std::cerr << test_title << " failed. Wrong exception thrown." << '\n';
            std::cerr << "Wrong error message: " << e.what() << std::endl;
        }

        print_arguments(arguments...);
        tests_passed = false;
    }

    bool files_are_identical(const std::string& file_name1, const std::string& file_name2) noexcept;
    size_t move_count(const Board& board, size_t maximum_depth) noexcept;
    bool run_board_tests(const std::string& file_name);
    bool all_moves_legal(Board& board, const std::vector<std::string>& moves) noexcept;
    bool move_is_illegal(const Board& board, const std::string& move) noexcept;

    void move_attack_indices_are_consistenly_defined(bool& tests_passed);

    void squares_with_unique_coordinates_have_unique_indices(bool& tests_passed);
    void constructed_squares_retain_coordinates(bool& tests_passed);
    void pieces_can_be_constructed_from_piece_data(bool& tests_passed);
    void pieces_can_be_constructed_from_fen_symbols(bool& tests_passed);
    void no_pair_of_adjacent_squares_are_the_same_color(bool& tests_passed);
    void difference_between_two_squares_added_to_first_square_gives_second_square(bool& tests_passed);
    void all_squares_yields_all_squares(bool& tests_passed);

    void algebraic_notation_resolves_ambiguous_moves(bool& tests_passed);

    void repeating_board_position_three_times_results_in_threefold_game_result(bool& tests_passed);
    void one_hundred_ply_with_no_pawn_or_capture_move_yields_fifty_move_game_result(bool& tests_passed);

    void derived_moves_applied_to_earlier_board_result_in_later_board(bool& tests_passed);
    void identical_boards_have_identical_hashes(bool& tests_passed);
    void boards_with_different_en_passant_targets_have_different_hashes(bool& tests_passed);

    void same_board_position_with_castling_rights_lost_by_different_methods_results_in_same_board_hash(bool& tests_passed);
    void same_board_position_with_different_castling_rights_has_different_hash(bool& tests_passed);

    void genome_loaded_from_file_writes_identical_file(bool& tests_passed);
    void self_swapped_minimax_ai_is_unchanged(bool& tests_passed);
    void self_assigned_minimax_ai_is_unchanged(bool& tests_passed);

    void castling_possible_gene_tests(bool& tests_passed);
    void freedom_to_move_gene_tests(bool& tests_passed);
    void king_confinement_gene_tests(bool& tests_passed);
    void king_protection_gene_tests(bool& tests_passed);
    void opponent_pieces_targeted_gene_tests(bool& tests_passed);
    void pawn_advancement_gene_tests(bool& tests_passed);
    void passed_pawn_gene_tests(bool& tests_passed);
    void total_force_gene_tests(bool& tests_passed);
    void stacked_pawns_gene_tests(bool& tests_passed);
    void pawn_islands_gene_tests(bool& tests_passed);
    void checkmate_material_gene_tests(bool& tests_passed);
    void sphere_of_influence_gene_tests(bool& tests_passed);
    void pawn_structure_gene_tests(bool& tests_passed);

    void game_progress_on_new_board_is_zero(bool& tests_passed);
    void game_progress_where_one_side_has_only_king_is_one(bool& tests_passed);
    void game_progress_with_one_queen_removed_makes_sense(bool& tests_passed);

    void split_and_join_are_inverse_operations(bool& tests_passed);
    void commas_as_thousands_separators_correctly_placed(bool& tests_passed);

    void specified_time_added_to_clock_after_specified_number_of_punches(bool& tests_passed);
    void clock_with_increment_gets_time_added_on_every_punch(bool& tests_passed);

    void midgame_node_result_values_compare_correctly(bool& tests_passed);
    void midgame_alpha_beta_values_compare_correctly_with_node_values(bool& tests_passed);
    void alpha_beta_result_values_compare_in_line_with_algorithm(bool& tests_passed);
    void alpha_and_beta_value_comparisons_fit_algorithm_definitions(bool& tests_passed);
    void endgame_node_result_tests(bool& tests_passed);

    void average_moves_left_matches_precalculated_value(bool& tests_passed);
    void average_moves_left_returns_finite_result_after_zero_moves(bool& tests_passed);
    void average_moves_left_returns_finite_result_after_one_move(bool& tests_passed);

    void math_normalize_test(bool& tests_passed);
    void scoped_push_back_works_as_advertised(bool& tests_passed);
    void has_exactly_n_works_as_advertised(bool& tests_passed);
}

bool run_tests()
{
    bool tests_passed = true;

    move_attack_indices_are_consistenly_defined(tests_passed);

    pieces_can_be_constructed_from_piece_data(tests_passed);
    pieces_can_be_constructed_from_fen_symbols(tests_passed);

    test_result(tests_passed, Square{'a', 1}.color() == Square_Color::BLACK, "Square a1 is not black.");
    no_pair_of_adjacent_squares_are_the_same_color(tests_passed);

    squares_with_unique_coordinates_have_unique_indices(tests_passed);
    constructed_squares_retain_coordinates(tests_passed);
    difference_between_two_squares_added_to_first_square_gives_second_square(tests_passed);
    all_squares_yields_all_squares(tests_passed);

    algebraic_notation_resolves_ambiguous_moves(tests_passed);

    repeating_board_position_three_times_results_in_threefold_game_result(tests_passed);
    one_hundred_ply_with_no_pawn_or_capture_move_yields_fifty_move_game_result(tests_passed);

    derived_moves_applied_to_earlier_board_result_in_later_board(tests_passed);
    identical_boards_have_identical_hashes(tests_passed);
    boards_with_different_en_passant_targets_have_different_hashes(tests_passed);

    test_result(tests_passed, run_board_tests("testing/board_tests.txt"), "");

    same_board_position_with_castling_rights_lost_by_different_methods_results_in_same_board_hash(tests_passed);
    same_board_position_with_different_castling_rights_has_different_hash(tests_passed);


    genome_loaded_from_file_writes_identical_file(tests_passed);
    self_swapped_minimax_ai_is_unchanged(tests_passed);
    self_assigned_minimax_ai_is_unchanged(tests_passed);

    function_should_not_throw(tests_passed, "Minimax_AI ctor",
                              []()
                              {
                                  const auto file_name = "genome_example.txt";
                                  return Minimax_AI{file_name, find_last_id(file_name)};
                              });

    castling_possible_gene_tests(tests_passed);
    freedom_to_move_gene_tests(tests_passed);
    king_confinement_gene_tests(tests_passed);
    king_protection_gene_tests(tests_passed);
    opponent_pieces_targeted_gene_tests(tests_passed);
    pawn_advancement_gene_tests(tests_passed);
    passed_pawn_gene_tests(tests_passed);
    sphere_of_influence_gene_tests(tests_passed);
    total_force_gene_tests(tests_passed);
    stacked_pawns_gene_tests(tests_passed);
    pawn_islands_gene_tests(tests_passed);
    checkmate_material_gene_tests(tests_passed);
    pawn_structure_gene_tests(tests_passed);

    game_progress_on_new_board_is_zero(tests_passed);
    game_progress_where_one_side_has_only_king_is_one(tests_passed);
    game_progress_with_one_queen_removed_makes_sense(tests_passed);

    function_should_throw<Missing_Genome_Data>(tests_passed, "Missing gene data", [](){ return Piece_Strength_Gene().read_from("testing/missing_data_genome.txt");});
    function_should_throw<Duplicate_Genome_Data>(tests_passed, "Duplicate gene data", [](){ return Sphere_of_Influence_Gene().read_from("testing/duplicate_data_genome.txt");});

    test_function(tests_passed, "Strip single-character comments", "a", String::strip_comments, "   a    #     b", "#");
    test_function(tests_passed, "Strip block comments", "a c", String::strip_block_comment, "   a    {    b    }    c   {   d  }   ", "{", "}");

    function_should_throw<std::invalid_argument>(tests_passed, "Bad block comments", String::strip_block_comment, "   a    }    b    {    c   {   d  }   ", "{", "}");
    function_should_throw<std::invalid_argument>(tests_passed, "Bad block comments", String::strip_block_comment, "   a        b    {    c      d     ", "{", "}");

    test_function(tests_passed, "Delimited text extraction", "a(b", String::extract_delimited_text, "(a(b))", "(", ")");

    test_function(tests_passed, "Nested block deletion", "ae", String::strip_nested_block_comments, "a(b(c)d)e", "(", ")");
    function_should_throw<std::invalid_argument>(tests_passed, "Invalid nested block delimiters", String::strip_nested_block_comments, "", "??", "???");

    test_function(tests_passed, "Strip multicharacter comment", "a", String::strip_comments, "a // b", "//");
    test_function(tests_passed, "Multicharacter block comment", "a c", String::strip_block_comment, "a /* b  */ c", "/*", "*/");
    test_function(tests_passed, "String::starts_with()", true, String::starts_with, "abcdefg", "abc");
    test_function(tests_passed, "String::starts_with()", false, String::starts_with, "abcdefg", "abd");
    test_function(tests_passed, "String::lowercase()", "abc def", String::lowercase, "AbC dEf");
    test_function(tests_passed, "String::add_to_file_name() with dot", "a-b.c", String::add_to_file_name, "a.c", "-b");
    test_function(tests_passed, "String::add_to_file_name() with no dot", "a-b", String::add_to_file_name, "a", "-b");

    using vs = std::vector<std::string>;
    test_function(tests_passed, "String::split()", vs{"a", "b", "c", "d", "e"}, String::split, "\t a b c d e ", "", -1);
    test_function(tests_passed, "Split on comma", vs{"", ""}, String::split, ",", ",", 1);
    test_function(tests_passed, "Ellipses split", vs{"", "a", "b", "c", "d", ""}, String::split, "..a..b..c..d..", "..", -1);
    test_function(tests_passed, "Ellipses split", vs{"", "a", "b", "c", "d.."}, String::split, "..a..b..c..d..", "..", 4);
    test_function(tests_passed, "Ellipses split", vs{"", "a", "b", "c", "d", ""}, String::split, "..a..b..c..d..", "..", 5);
    test_function(tests_passed, "Empty string split", vs{}, String::split, "", " ", 1000);
    split_and_join_are_inverse_operations(tests_passed);

    test_function(tests_passed, "Format integer (zero)",  "0", String::format_integer<int>,  0, ",");
    test_function(tests_passed, "Format integer (-zero)", "0", String::format_integer<int>, -0, ",");
    commas_as_thousands_separators_correctly_placed(tests_passed);

    function_should_not_throw(tests_passed, "Non-throwing string to size_t", String::to_number<size_t>, "123");
    function_should_not_throw(tests_passed, "Non-throwing string to size_t with whitespace", String::to_number<size_t>, " 456  ");
    function_should_throw<std::invalid_argument>(tests_passed, "Throwing string to size_t", String::to_number<size_t>, "78x9");
    function_should_throw<std::invalid_argument>(tests_passed, "Throwing empty string to int", String::to_number<int>, "");
    function_should_throw<std::invalid_argument>(tests_passed, "Throwing on too large value", String::to_number<short>, "99999");

    test_function(tests_passed, "Singular pluralizing", "1 cat", String::pluralize, 1, "cat");
    test_function(tests_passed, "Multiple pluralizing", "4 cats", String::pluralize, 4, "cat");

    test_function(tests_passed,
                  "PGN comment removal",
                  "49. f8=N Bxf8",
                  String::remove_pgn_comments,
                  "49. f8=N (49. f8=N Bxf8 50. Kf3 {-12.60}) Bxf8 (49. ... Bxf8 50. Kf1 (50. Kf3 Rd7 {16.43}) {16.43}) { Test block } ; Test line");


    average_moves_left_matches_precalculated_value(tests_passed);
    average_moves_left_returns_finite_result_after_zero_moves(tests_passed);
    average_moves_left_returns_finite_result_after_one_move(tests_passed);

    math_normalize_test(tests_passed);

    specified_time_added_to_clock_after_specified_number_of_punches(tests_passed);
    clock_with_increment_gets_time_added_on_every_punch(tests_passed);

    midgame_node_result_values_compare_correctly(tests_passed);
    midgame_alpha_beta_values_compare_correctly_with_node_values(tests_passed);
    alpha_beta_result_values_compare_in_line_with_algorithm(tests_passed);
    alpha_and_beta_value_comparisons_fit_algorithm_definitions(tests_passed);
    endgame_node_result_tests(tests_passed);

    scoped_push_back_works_as_advertised(tests_passed);
    has_exactly_n_works_as_advertised(tests_passed);


    std::cout << (tests_passed ? "All tests passed." : "Tests failed.") << std::endl;
    return tests_passed;
}

void run_speed_tests()
{
    std::cout << "Gene scoring speed ..." << std::endl;
    auto test_genes_file_name = "testing/test_genome.txt";

    auto castling_possible_gene = Castling_Possible_Gene();
    castling_possible_gene.read_from(test_genes_file_name);
    const auto freedom_to_move_gene = Freedom_To_Move_Gene();
    auto king_confinement_gene = King_Confinement_Gene();
    king_confinement_gene.read_from(test_genes_file_name);
    const auto king_protection_gene = King_Protection_Gene();
    auto piece_strength_gene = Piece_Strength_Gene();
    piece_strength_gene.read_from(test_genes_file_name);
    const auto opponent_pieces_targeted_gene = Opponent_Pieces_Targeted_Gene(&piece_strength_gene);
    const auto pawn_advancement_gene = Pawn_Advancement_Gene();
    const auto passed_pawn_gene = Passed_Pawn_Gene();
    auto sphere_of_influence_gene = Sphere_of_Influence_Gene();
    sphere_of_influence_gene.read_from(test_genes_file_name);
    const auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
    const auto stacked_pawns_gene = Stacked_Pawns_Gene();
    const auto pawn_islands_gene = Pawn_Islands_Gene();
    const auto checkmate_material_gene = Checkmate_Material_Gene();
    auto pawn_structure_gene = Pawn_Structure_Gene();
    pawn_structure_gene.read_from(test_genes_file_name);

    auto performance_board = Board();
    for(const auto& move : String::split("e4 e6 d4 h5 d5 b5 Qf3 g6 Be2 Bg7 Bd2 h4 Nh3 Na6 Nc3 b4 Nf4 Nc5 Nd3 Na4 Ne5 Nb6 Qd3 Qe7 Qe3 Ba6 Qf3 Nf6 Qe3 h3 Qf4 Qc5 Qf3 Qe7 O-O-O"))
    {
        performance_board.play_move(move);
    }

    if(performance_board.fen() != "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/2KR3R b kq - 5 18")
    {
        throw std::logic_error("Different FEN from previous tests.");
    }

    const std::vector<const Gene*> performance_genome = {&castling_possible_gene,
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
                                                         &pawn_structure_gene};

#ifdef NDEBUG
    const auto number_of_tests = 1'000'000;
    const auto time_unit = "us";
#else
    const auto number_of_tests = 1'000;
    const auto time_unit = "ms";
#endif // NDEBUG
    std::vector<std::pair<std::chrono::steady_clock::duration, std::string>> timing_results;
    const auto all_genes_start = std::chrono::steady_clock::now();
    for(const auto gene : performance_genome)
    {
        auto score = 0.0;
        const auto gene_start = std::chrono::steady_clock::now();
        for(int i = 1; i <= number_of_tests; ++i)
        {
            const auto side = performance_board.whose_turn();
            score += gene->evaluate(performance_board, opposite(side), performance_board.played_ply_count(), 20);
        }
        timing_results.emplace_back(std::chrono::steady_clock::now() - gene_start, gene->name());
        (void)score;
    }
    timing_results.emplace_back(std::chrono::steady_clock::now() - all_genes_start, "Complete gene scoring");

    std::cout << "Board::play_move() speed ..." << std::endl;
    const auto game_time_start = std::chrono::steady_clock::now();
    Board speed_board;
    const auto speed_board_spare = speed_board;
    for(auto i = 0; i < number_of_tests; ++i)
    {
        const auto move = Random::random_element(speed_board.legal_moves());
        const auto move_result = speed_board.play_move(*move);
        if(move_result.game_has_ended())
        {
            speed_board = speed_board_spare;
        }
    }
    timing_results.emplace_back(std::chrono::steady_clock::now() - game_time_start, "Board::play_move()");
    const auto board_play_move_time = timing_results.back().first;

    std::cout << "Board::play_move() with copy speed ..." << std::endl;
    const auto copy_game_start = std::chrono::steady_clock::now();
    Board copy_speed_board;
    for(auto i = 0; i < number_of_tests; ++i)
    {
        const auto move = Random::random_element(copy_speed_board.legal_moves());
        auto copy = copy_speed_board;
        auto move_result = copy.play_move(*move);
        if(move_result.game_has_ended())
        {
            copy_speed_board = Board{};
        }
        else
        {
            copy_speed_board = copy;
        }
    }
    timing_results.emplace_back(std::chrono::steady_clock::now() - copy_game_start, "Board::play_move() with copy");

    std::cout << "Board::quiescent() speed ... " << std::flush;
    const auto quiescent_time_start = std::chrono::steady_clock::now();
    Board quiescent_board;
    size_t move_count = 0;
    for(auto i = 0; i < number_of_tests; ++i)
    {
        while(true)
        {
            const auto move = Random::random_element(quiescent_board.legal_moves());
            const auto move_result = quiescent_board.play_move(*move);
            ++move_count;
            if(move_result.game_has_ended())
            {
                quiescent_board = Board{};
            }
            else if(quiescent_board.attacked_by(move->end(), quiescent_board.whose_turn()))
            {
                break;
            }
        }

        const auto quiescent_result_board = quiescent_board.quiescent({1.0, 5.0, 3.0, 3.0, 8.0, 100.0});
    }
    const auto quiescent_time = std::chrono::steady_clock::now() - quiescent_time_start;
    timing_results.emplace_back(quiescent_time - (board_play_move_time*move_count)/number_of_tests, "Board::quiescent()");
    std::cout << "(non-quiescent moves = " << String::format_integer(move_count, ",") << ")" << std::endl;

    std::sort(timing_results.begin(), timing_results.end());
    const auto name_width = int(std::max_element(timing_results.begin(), timing_results.end(),
                                                 [](const auto& x, const auto& y){ return x.second.size() < y.second.size(); })->second.size());
    std::cout << "\n" << std::setw(name_width) << "Test Item" << "   " << "Time (" << time_unit << ")";
    std::cout << "\n" << std::setw(name_width) << "---------" << "   " << "---------" << std::endl;
    for(const auto& [time, name] : timing_results)
    {
        std::cout << std::setw(name_width) << name << " = " << std::chrono::duration<double>(time).count() << std::endl;
    }
}

bool run_perft_tests()
{
    // Count game tree leaves (perft) to given depth to validate move generation
    // (downloaded from http://www.rocechess.ch/perft.html)
    // (leaves from starting positions also found at https://oeis.org/A048987)

    std::vector<int> tests_failed;
    auto perft_suite_input = std::ifstream("testing/perftsuite.epd");
    std::vector<std::string> lines;
    for(std::string input_line; std::getline(perft_suite_input, input_line);)
    {
        lines.push_back(input_line);
    }
    std::sort(lines.begin(), lines.end(),
              [](const auto& x, const auto& y)
              {
                  auto f = [](const auto& s) { return String::to_number<size_t>(String::split(s).back()); };
                  return f(x) < f(y);
              });

    auto test_number = 0;
    size_t legal_moves_counted = 0;
    const auto time_at_start_of_all = std::chrono::steady_clock::now();
    const auto test_count_space = int(std::to_string(lines.size()).size());
    for(const auto& line : lines)
    {
        const auto time_at_start = std::chrono::steady_clock::now();
        const auto line_parts = String::split(line, ";");
        const auto fen = line_parts.front();
        std::cout << '[' << std::setw(test_count_space) << ++test_number << '/' << lines.size() << "] " << fen << std::flush;
        const auto perft_board = Board(fen);
        const auto tests = std::vector<std::string>(line_parts.begin() + 1, line_parts.end());
        for(const auto& test : tests)
        {
            const auto depth_leaves = String::split(test);
            assert(depth_leaves.size() == 2);
            assert(depth_leaves.front().front() == 'D');
            const auto depth = String::to_number<size_t>(depth_leaves.front().substr(1));
            const auto expected_leaves = String::to_number<size_t>(depth_leaves.back());
            const auto leaf_count = move_count(perft_board, depth);
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
        const auto time_at_end = std::chrono::steady_clock::now();
        const auto time_for_test = time_at_end - time_at_start;
        const auto time_so_far = time_at_end - time_at_start_of_all;
        std::cout << std::chrono::duration<double>(time_for_test).count() << " / " << std::chrono::duration<double>(time_so_far).count() << std::endl;
    }

    const auto time = std::chrono::duration<double>(std::chrono::steady_clock::now() - time_at_start_of_all);
    std::cout << "Perft time: " << time.count() << " seconds" << std::endl;
    std::cout << "Legal moves counted: " << String::format_integer(legal_moves_counted, ",") << std::endl;
    std::cout << "Move generation rate: " << String::format_integer(int(double(legal_moves_counted)/time.count()), ",") << " moves/second." << std::endl;
    if( ! tests_failed.empty())
    {
        std::cout << String::pluralize(int(tests_failed.size()), "Test") << " failed: ";
        for(auto t : tests_failed)
        {
            std::cout << t << " ";
        }
        std::cout << std::endl;
    }
    return tests_failed.empty();
}

namespace
{
    bool test_result(bool& all_tests_passed, const bool expected_result, const std::string& fail_message) noexcept
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
                    const auto split1 = String::split(line1, ":", 1);
                    const auto split2 = String::split(line2, ":", 1);
                    if(split1.front() == split2.front())
                    {
                        const auto num1 = String::to_number<double>(split1.back());
                        const auto num2 = String::to_number<double>(split2.back());
                        if(std::abs((num1 - num2)/(num1 + num2)) < 1e-5)
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

    size_t move_count(const Board& board, const size_t maximum_depth) noexcept
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
        for(const auto move : board.legal_moves())
        {
            auto next_board = board;
            next_board.play_move(*move);
            count += move_count(next_board, maximum_depth - 1);
        }

        return count;
    }

    bool run_board_tests(const std::string& file_name)
    {
        auto input = std::ifstream(file_name);
        if( ! input)
        {
            std::cerr << "Could not open board test file.\n";
            return false;
        }

        auto all_tests_passed = true;

        for(std::string line; std::getline(input, line);)
        {
            line = String::strip_comments(line, "#");
            if(line.empty())
            {
                continue;
            }

            auto test_assert = [&line, &all_tests_passed](bool expected)
                               {
                                    if( ! expected)
                                    {
                                        std::cerr << "Malformed test: " << line << '\n';
                                        all_tests_passed = false;
                                    }
                                    return expected;
                                };

            auto specification = String::split(line, "|");
            if( ! test_assert(specification.size() >= 2)) { continue; }

            std::transform(specification.begin(), specification.end(), specification.begin(), String::remove_extra_whitespace);

            const auto test_type = String::lowercase(specification.at(0));
            const auto board_fen = specification.at(1);
            auto test_passed = true;

            if(test_type == "illegal position")
            {
                function_should_throw<std::invalid_argument>(test_passed, "", [](const std::string& s) { return Board{s}; }, board_fen);
                test_result(all_tests_passed, test_passed, line + " -- FAILED\n");
                continue;
            }

            auto board = board_fen == "start" ? Board{} : Board{board_fen};
            if( ! test_assert(specification.size() >= 3)) { continue; }

            if(test_type == "all moves legal")
            {
                const auto moves = String::split(specification.at(2));
                test_result(test_passed, all_moves_legal(board, moves), "");
            }
            else if(test_type == "last move illegal")
            {
                auto moves = String::split(specification.at(2));
                if( ! test_assert( ! moves.empty())) { continue; }
                const auto last_move = moves.back();
                moves.pop_back();
                test_result(test_passed, all_moves_legal(board, moves) && move_is_illegal(board, last_move), "");
            }
            else if(test_type == "pinned piece")
            {
                const auto square_result = String::split(specification.at(2));
                if( ! test_assert(square_result.size() == 2)) { continue; }
                if( ! test_assert(square_result.front().size() == 2)) { continue; }

                const auto square = String::lowercase(square_result.front());
                const auto expected_result = String::lowercase(square_result.back());

                if( ! test_assert(expected_result == "true" || expected_result == "false")) { continue; }
                const auto expected_bool = (expected_result == "true");
                test_result(test_passed, board.piece_is_pinned(Square(square.front(), square.back() - '0')) == expected_bool,
                            "Expected result of " + square + " being pinned: " + expected_result);
            }
            else if(test_type == "move count")
            {
                if( ! test_assert(specification.size() == 4)) { continue; }
                const auto moves = String::split(specification.at(2));
                const auto expected_count = String::to_number<size_t>(specification.back());
                test_result(test_passed, all_moves_legal(board, moves) && board.legal_moves().size() == expected_count,
                            "Legal moves counted: " + std::to_string(board.legal_moves().size()) + "; Expected: " + std::to_string(expected_count));
            }
            else if(test_type == "checkmate material")
            {
                const auto result_text = specification.back();
                if( ! test_assert(result_text == "true" || result_text == "false")) { continue; }
                const auto expected_result = (result_text == "true");
                test_result(test_passed, board.enough_material_to_checkmate() == expected_result,
                            std::string("This board does") + (expected_result ? "" : " not") + " have enough material to checkmate.");
            }
            else if(test_type == "king in check")
            {
                if( ! test_assert(specification.size() == 4)) { continue; }
                const auto moves = String::split(specification.at(2));
                const auto expected_answer = String::lowercase(specification.back());
                if( ! test_assert(expected_answer == "true" || expected_answer == "false")) { continue; }
                const auto expected_result = expected_answer == "true";
                test_result(test_passed, all_moves_legal(board, moves) && board.king_is_in_check() == expected_result,
                            std::string("King is ") +
                                (expected_result ? "not " : "") +
                                "in check when it should " +
                                (expected_result ? "" : "not ") +
                                "be in check.");
            }
            else if(test_type == "quiescent")
            {
                if( ! test_assert(specification.size() == 4)) { continue; }
                const auto moves = String::split(specification.at(2));
                test_result(test_passed, all_moves_legal(board, moves), "Bad test: Illegal moves");
                auto actual_result_board = board;
                for(const auto move : board.quiescent({1.0, 5.0, 3.0, 3.0, 8.0, 100.0}))
                {
                    actual_result_board.play_move(*move);
                }
                for(const auto& quiescent_move : String::split(specification.at(3)))
                {
                    board.play_move(quiescent_move);
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

                    game_has_ended = board.play_move(move).game_has_ended();
                });
        }

        return result;
    }

    bool move_is_illegal(const Board& board, const std::string& move) noexcept
    {
        bool result = true;
        function_should_throw<Illegal_Move>(result, move + " should be illegal", [&](){ board.interpret_move(move); });
        return result;
    }

    void move_attack_indices_are_consistenly_defined(bool& tests_passed)
    {
        for(size_t i = 0; i < 16; ++i)
        {
            const auto step = Move::attack_direction_from_index(i);
            const auto start = Square{'e', 4};
            const auto step_index = Move(start, start + step).attack_index();
            test_result(tests_passed, step_index == i, "Direction-index mismatch: " + std::to_string(i) + " --> " + std::to_string(step_index));
        }
    }

    void squares_with_unique_coordinates_have_unique_indices(bool& tests_passed)
    {
        std::array<bool, 64> visited{};
        for(char file = 'a'; file <= 'h'; ++file)
        {
            for(int rank = 1; rank <= 8; ++rank)
            {
                const auto square = Square{file, rank};
                test_result(tests_passed, ! visited[square.index()], "Multiple squares result in same index." + square.text());
                visited[square.index()] = true;
            }
        }
        test_result(tests_passed, std::all_of(visited.begin(), visited.end(), [](auto x) { return x; }), "Not all indices visited by iterating through all squares.");
    }

    void constructed_squares_retain_coordinates(bool& tests_passed)
    {
        for(char file = 'a'; file <= 'h'; ++file)
        {
            for(int rank = 1; rank <= 8; ++rank)
            {
                const auto square = Square{file, rank};
                test_result(tests_passed,
                            square.file() == file && square.rank() == rank,
                            std::string{"Square constructed with "} + file + std::to_string(rank) + " results in " + square.text());
            }
        }
    }

    void pieces_can_be_constructed_from_piece_data(bool& tests_passed)
    {
        for(auto type_index = 0; type_index <= static_cast<int>(Piece_Type::KING); ++type_index)
        {
            const auto type = static_cast<Piece_Type>(type_index);
            for(auto color : {Piece_Color::BLACK, Piece_Color::WHITE})
            {
                const auto piece = Piece{color, type};
                const auto piece2 = Piece{piece.color(), piece.type()};
                test_result(tests_passed, piece == piece2, std::string("Inconsistent construction for ") + piece.fen_symbol() + " --> " + piece2.fen_symbol());
            }
        }
    }

    void pieces_can_be_constructed_from_fen_symbols(bool& tests_passed)
    {
        for(auto type_index = 0; type_index <= static_cast<int>(Piece_Type::KING); ++type_index)
        {
            const auto type = static_cast<Piece_Type>(type_index);
            for(auto color : {Piece_Color::BLACK, Piece_Color::WHITE})
            {
                const auto piece = Piece{color, type};
                const auto piece2 = Piece(piece.fen_symbol());
                test_result(tests_passed, piece == piece2, std::string("Inconsistent FEN construction for ") + piece.fen_symbol() + " --> " + piece2.fen_symbol());
            }
        }
    }

    void no_pair_of_adjacent_squares_are_the_same_color(bool& tests_passed)
    {
        for(const auto square1 : Square::all_squares())
        {
            for(const auto square2 : Square::all_squares())
            {
                const auto diff = square1 - square2;
                const auto df = std::abs(diff.file_change);
                const auto dr = std::abs(diff.rank_change);
                if((dr == 0 && df == 1) || (df == 0 && dr == 1)) // square are adjacent in same row or column
                {
                    test_result(tests_passed, square1.color() != square2.color(), "Adjacent squares " + square1.text() + " and " + square2.text() + " have same color.");
                }
            }
        }
    }

    void difference_between_two_squares_added_to_first_square_gives_second_square(bool& tests_passed)
    {
        for(const auto a : Square::all_squares())
        {
            for(const auto b : Square::all_squares())
            {
                test_result(tests_passed,
                            a + (b - a) == b,
                            "Square arithetic problem: " +
                            a.text() + " + (" + b.text() + " - " + a.text() + ") != " + b.text());
            }
        }
    }

    void all_squares_yields_all_squares(bool& tests_passed)
    {
        std::array<bool, 64> squares_visited{};
        for(const auto square : Square::all_squares())
        {
            test_result(tests_passed, ! squares_visited[square.index()], "Sqaure " + square.text() + " already visited.");
            squares_visited[square.index()] = true;
        }
        test_result(tests_passed,
                    std::all_of(squares_visited.begin(), squares_visited.end(), [](auto tf) { return tf; }),
                    "Square iterator missed some squares.");
    }

    void test_ambiguous_move(bool& tests_passed, const std::string& fen, const std::string& move_text, const std::string& start_square, const std::string& end_square)
    {
        const auto board = Board(fen);
        const auto& move_list = board.legal_moves();
        const auto find_move_text = [&board, &move_text](const Move* const move) { return move->algebraic(board) == move_text; };
        const auto found_move = std::find_if(move_list.begin(), move_list.end(), find_move_text);

        test_result(tests_passed, found_move != move_list.end(), "Ambiguous move notation not found: " + move_text);
        if(found_move != move_list.end())
        {
            test_result(tests_passed, (*found_move)->start().text() == start_square, move_text + " does not start on square " + start_square + ".");
            test_result(tests_passed, (*found_move)->end().text() == end_square, move_text + " does not end on square " + end_square + ".");
            test_result(tests_passed, std::find_if(std::next(found_move), move_list.end(), find_move_text) == move_list.end(), "Multiple moves with algebraic text: " + move_text);
        }
    }

    void algebraic_notation_resolves_ambiguous_moves(bool& tests_passed)
    {
        // Adapted from https://en.wikipedia.org/wiki/Algebraic_notation_(chess)#Disambiguating_moves

        test_ambiguous_move(tests_passed, "1k1r3r/8/8/R7/4Q2Q/8/8/RK5Q b - - 0 1", "Rdf8", "d8", "f8");
        test_ambiguous_move(tests_passed, "1k1r3r/8/8/R7/4Q2Q/8/8/RK5Q w - - 0 1", "R1a3", "a1", "a3");
        test_ambiguous_move(tests_passed, "1k1r3r/8/8/R7/4Q2Q/8/8/RK5Q w - - 0 1", "Qh4e1", "h4", "e1");
    }

    void repeating_board_position_three_times_results_in_threefold_game_result(bool& tests_passed)
    {
        auto repeat_board = Board();
        Game_Result repeat_result;
        auto repeat_move_count = 0;

        for(int repetition = 0; repetition < 2; ++repetition)
        {
            for(const auto& move : {"Nc3", "Nc6", "Nb1", "Nb8"})
            {
                ++repeat_move_count;
                repeat_result = repeat_board.play_move(move);
                if(repeat_result.game_has_ended())
                {
                    test_result(tests_passed, repeat_move_count == 8, "Threefold repetition triggered early.");
                }
            }
        }
        test_result(tests_passed, repeat_result.ending_reason() == "Threefold repetition", "Threefold stalemate not triggered.");
    }

    void one_hundred_ply_with_no_pawn_or_capture_move_yields_fifty_move_game_result(bool& tests_passed)
    {
        auto fifty_move_board = Board();
        auto fifty_move_result = Game_Result();
        for(int move_counter = 1; move_counter <= 100; ++move_counter)
        {
            auto move_chosen = false;
            for(const auto move : fifty_move_board.legal_moves())
            {
                if(fifty_move_board.move_captures(*move))
                {
                    continue;
                }

                if(std::as_const(fifty_move_board).piece_on_square(move->start()).type() == Piece_Type::PAWN)
                {
                    continue;
                }

                auto next_board = fifty_move_board;
                auto result = next_board.play_move(*move);

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

                fifty_move_result = fifty_move_board.play_move(*move);
                move_chosen = true;
                break;
            }

            test_result(tests_passed, move_chosen, "Unable to choose next move (moves made = " + std::to_string(move_counter) + ").");
        }
        test_result(tests_passed, fifty_move_result.ending_reason() == "50-move limit", "50-move draw test result: Got: " + fifty_move_result.ending_reason() + " instead.");
    }

    void derived_moves_applied_to_earlier_board_result_in_later_board(bool& tests_passed)
    {
        Board move_derivation_board;
        const auto goal_board = Board{"rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"};
        const auto derived_moves = move_derivation_board.derive_moves(goal_board);
        test_result(tests_passed, derived_moves.size() == 2, "Wrong number of moves derived. Got " + std::to_string(derived_moves.size()));
        for(const auto move : derived_moves)
        {
            move_derivation_board.play_move(*move);
        }
        test_result(tests_passed, move_derivation_board.fen() == goal_board.fen(), "Wrong moves derived. " + move_derivation_board.fen() + " != " + goal_board.fen());
    }

    void identical_boards_have_identical_hashes(bool& tests_passed)
    {
        Board board;
        test_result(tests_passed, board.board_hash() == Board{board.fen()}.board_hash(), "Standard starting board hashes do not match.");
        std::vector<std::string> moves;
        auto maximum_move_count =
        #ifdef NDEBUG
            1'000'000;
        #else
            1'000;
        #endif
        for(auto move_count = 1; move_count <= maximum_move_count; ++move_count)
        {
            const auto& move_list = board.legal_moves();
            if(move_list.empty())
            {
                board = {};
                moves = {};
            }
            else
            {
                const auto move = Random::random_element(move_list);
                moves.push_back(move->algebraic(board));
                board.play_move(*move);
                const auto identical_board = Board(board.fen());
                if(board.board_hash() != identical_board.board_hash())
                {
                    tests_passed = false;
                    std::cerr << "Boards do not have equal hashes: " << board.fen() << "\n"
                              << "                                 " << identical_board.fen() << "\n"
                              << "Move count: " << move_count << std::endl;
                    std::cerr << "Moves: " << String::join(moves.begin(), moves.end(), " ") << std::endl;
                    board.compare_hashes(identical_board);
                    break;
                }
            }
        }
    }

    void boards_with_different_en_passant_targets_have_different_hashes(bool& tests_passed)
    {
        Board board;
        for(const auto move : {"e4", "a6", "e5", "f5"})
        {
            board.play_move(move);
        }
        const auto hash  = board.board_hash();
        const auto fen = board.fen();

        for(const auto move : {"Nc3", "Nc6", "Nb1", "Nb8"})
        {
            board.play_move(move);
        }

        test_result(tests_passed, hash != board.board_hash(), "Change in en passant target should result in different hash.\n" + fen + "\n" + board.fen());
    }

    void same_board_position_with_castling_rights_lost_by_different_methods_results_in_same_board_hash(bool& tests_passed)
    {
        const auto castling_hash_board = Board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

        // Lose all castling rights due to king moving
        auto just_kings_move_board = castling_hash_board;
        for(auto move : {"Ke2", "Ke7", "Ke1", "Ke8"})
        {
            just_kings_move_board.play_move(move);
        }

        // Lose all castling rights due to rooks moving
        auto just_rooks_move_board = castling_hash_board;
        for(auto move : {"Ra2", "Ra7", "Ra1", "Ra8", "Rh2", "Rh7", "Rh1", "Rh8"})
        {
            just_rooks_move_board.play_move(move);
        }

        test_result(tests_passed, just_kings_move_board.board_hash() == just_rooks_move_board.board_hash(), "Boards should have same hash after castling rights lost");
    }

    void same_board_position_with_different_castling_rights_has_different_hash(bool& tests_passed)
    {
        const auto castling_hash_board = Board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

        // Lose all castling rights due to king moving
        auto just_kings_move_board = castling_hash_board;
        for(auto move : {"Ke2", "Ke7", "Ke1", "Ke8"})
        {
            just_kings_move_board.play_move(move);
        }

        test_result(tests_passed, just_kings_move_board.board_hash() != castling_hash_board.board_hash(), "Boards should have different hashes with different castling rights");
    }

    void genome_loaded_from_file_writes_identical_file(bool& tests_passed)
    {
        const auto pool_file_name = "test_gene_pool.txt";
        const auto write_file_name = "test_genome_write.txt";
        const auto rewrite_file_name = "test_genome_rewrite.txt";
        remove(pool_file_name);
        remove(write_file_name);
        remove(rewrite_file_name);

        std::vector<Minimax_AI> test_pool(10);
        for(auto& ai : test_pool)
        {
            ai.mutate(100);
            ai.print(pool_file_name);
        }

        const auto& test_ai = Random::random_element(test_pool);
        test_ai.print(write_file_name);
        auto read_ai = Minimax_AI(pool_file_name, test_ai.id());
        read_ai.print(rewrite_file_name);

        if(test_result(tests_passed, files_are_identical(write_file_name, rewrite_file_name), "Genome loaded from gene pool file not preserved."))
        {
            remove(pool_file_name);
            remove(write_file_name);
            remove(rewrite_file_name);
        }
    }

    void self_swapped_minimax_ai_is_unchanged(bool& tests_passed)
    {
        auto self_swap_ai = Minimax_AI();
        self_swap_ai.mutate(100);
        const auto self_write_file_name = "self_original.txt";
        remove(self_write_file_name);
        self_swap_ai.print(self_write_file_name);

        std::swap(self_swap_ai, self_swap_ai);
        const auto self_swap_file_name = "self_swap.txt";
        remove(self_swap_file_name);
        self_swap_ai.print(self_swap_file_name);

        if(test_result(tests_passed, files_are_identical(self_write_file_name, self_swap_file_name), "Self-swap changed AI."))
        {
            remove(self_write_file_name);
            remove(self_swap_file_name);
        }
    }

    void self_assigned_minimax_ai_is_unchanged(bool& tests_passed)
    {
        auto self_assign_ai = Minimax_AI();
        self_assign_ai.mutate(100);
        auto self_write_file_name = "self_assign_original.txt";
        remove(self_write_file_name);
        self_assign_ai.print(self_write_file_name);

        const auto& copy_ai = self_assign_ai;
        self_assign_ai = copy_ai;
        auto self_assign_file_name = "self_assign_post_assignment.txt";
        remove(self_assign_file_name);
        self_assign_ai.print(self_assign_file_name);

        if(test_result(tests_passed, files_are_identical(self_write_file_name, self_assign_file_name), "Self-assignment changed AI."))
        {
            remove(self_write_file_name);
            remove(self_assign_file_name);
        }
    }


    void castling_possible_gene_tests(bool& tests_passed)
    {
        auto castling_possible_gene = Castling_Possible_Gene();
        castling_possible_gene.read_from("testing/test_genome.txt");

        auto castling_board = Board("rn2k3/8/8/8/8/8/8/R3K2R w KQq - 0 1");
        castling_possible_gene.test(tests_passed, castling_board, Piece_Color::WHITE, (0.8 + 0.2) / 1.0);

        castling_board.play_move("O-O");
        castling_possible_gene.test(tests_passed, castling_board, Piece_Color::WHITE, 0.8 / 1); // castled at depth 1
        castling_possible_gene.test(tests_passed, castling_board, Piece_Color::BLACK, 0.2 / (1.0 + 2.0 + 1.0));

        castling_board.play_move("Nc6");
        castling_board.play_move("Rab1");
        castling_board.play_move("O-O-O");
        castling_possible_gene.test(tests_passed, castling_board, Piece_Color::BLACK, 0.2 / 4); // castled at depth 4
    }

    void freedom_to_move_gene_tests(bool& tests_passed)
    {
        const auto freedom_to_move_gene = Freedom_To_Move_Gene();
        auto freedom_to_move_board = Board("5k2/8/8/8/4Q3/8/8/3K4 w - - 0 1");

        const auto freedom_to_move_white_score = 32.0/128.0;
        freedom_to_move_gene.test(tests_passed, freedom_to_move_board, Piece_Color::WHITE, freedom_to_move_white_score);

        freedom_to_move_board.play_move("Qd5");
        const auto freedom_to_move_black_score = 3.0/128.0;
        freedom_to_move_gene.test(tests_passed, freedom_to_move_board, Piece_Color::BLACK, freedom_to_move_black_score);
        freedom_to_move_gene.test(tests_passed, freedom_to_move_board, Piece_Color::WHITE, 0.0);
    }

    void king_confinement_gene_tests(bool& tests_passed)
    {
        auto king_confinement_gene = King_Confinement_Gene();
        king_confinement_gene.read_from("testing/test_genome.txt");

        const auto king_confinement_board = Board("k3r3/8/8/8/8/8/5PPP/7K w - - 0 1");
        const auto king_confinement_score = 3.0 / 64; // free squares (h1, g1, f1)
        king_confinement_gene.test(tests_passed, king_confinement_board, Piece_Color::WHITE, king_confinement_score);

        const auto king_confined_by_pawns_board = Board("k7/8/8/8/8/pppppppp/8/K7 w - - 0 1");
        const auto king_confined_by_pawns_score = 8.0 / 64; // free squares (a1-h1)
        king_confinement_gene.test(tests_passed, king_confined_by_pawns_board, Piece_Color::WHITE, king_confined_by_pawns_score);
    }

    void king_protection_gene_tests(bool& tests_passed)
    {
        const auto king_protection_gene = King_Protection_Gene();
        const auto king_protection_board = Board("k3r3/8/8/8/8/8/5PPP/7K w - - 0 1");
        const auto max_square_count = 8 + 7 + 7 + 7 + 6; // max_square_count in King_Protection_Gene.cpp
        const auto square_count = 7 + 1; // row attack along rank 1 + knight attack from g3
        const auto king_protection_score = double(max_square_count - square_count)/max_square_count;
        king_protection_gene.test(tests_passed, king_protection_board, Piece_Color::WHITE, king_protection_score);
    }

    void pawn_advancement_gene_tests(bool& tests_passed)
    {
        const auto pawn_advancement_gene = Pawn_Advancement_Gene();
        const auto pawn_advancement_board = Board("7k/4P3/3P4/2P5/1P6/P7/8/K7 w - - 0 1");
        const auto pawn_advancement_score = double(1 + 2 + 3 + 4 + 5) / (8 * 5);
        pawn_advancement_gene.test(tests_passed, pawn_advancement_board, Piece_Color::WHITE, pawn_advancement_score);
    }

    void passed_pawn_gene_tests(bool& tests_passed)
    {
        const auto passed_pawn_gene = Passed_Pawn_Gene();
        auto passed_pawn_board = Board("k1K5/8/8/3pP3/3P4/8/8/8 w - - 0 1");
        auto passed_pawn_score = (1.0 + 2.0 / 3.0) / 8;
        passed_pawn_gene.test(tests_passed, passed_pawn_board, Piece_Color::WHITE, passed_pawn_score);

        passed_pawn_board.play_move("Kd8");
        passed_pawn_score = (2.0 / 3.0) / 8;
        passed_pawn_gene.test(tests_passed, passed_pawn_board, Piece_Color::BLACK, passed_pawn_score);
    }

    void stacked_pawns_gene_tests(bool& tests_passed)
    {
        const auto stacked_pawns_gene = Stacked_Pawns_Gene();
        const auto stacked_pawns_board = Board("k7/8/8/8/P7/PP6/PPP5/K7 w - - 0 1");
        stacked_pawns_gene.test(tests_passed, stacked_pawns_board, Piece_Color::WHITE, -3.0 / 6);
    }

    void pawn_islands_gene_tests(bool& tests_passed)
    {
        const auto pawn_islands_gene = Pawn_Islands_Gene();
        const auto pawn_islands_board = Board("k7/8/8/8/8/8/P1PPP1PP/K7 w - - 0 1");
        pawn_islands_gene.test(tests_passed, pawn_islands_board, Piece_Color::WHITE, -3.0 / 4.0);
    }

    void checkmate_material_gene_tests(bool& tests_passed)
    {
        const auto checkmate_material_gene = Checkmate_Material_Gene();
        const auto checkmate_material_board = Board("k7/8/8/8/8/8/8/6RK w - - 0 1");
        checkmate_material_gene.test(tests_passed, checkmate_material_board, Piece_Color::WHITE, 1.0); // white can checkmate
        checkmate_material_gene.test(tests_passed, checkmate_material_board, Piece_Color::BLACK, 0.0); // black cannot
    }

    void opponent_pieces_targeted_gene_tests(bool& tests_passed)
    {
        auto target_piece_strength_gene = Piece_Strength_Gene();
        target_piece_strength_gene.read_from("testing/test_genome.txt");
        const auto piece_strength_normalizer = double(32 + 16 + 2 * 8 + 2 * 4 + 2 * 2 + 8 * 1);

        const auto opponent_pieces_targeted_gene = Opponent_Pieces_Targeted_Gene(&target_piece_strength_gene);
        const auto opponent_pieces_targeted_board = Board("k1K5/8/8/8/8/1rp5/nQb5/1q6 w - - 0 1");
        const auto opponent_pieces_targeted_score = (16 + 8 + 4 + 2 + 1) / piece_strength_normalizer;
        opponent_pieces_targeted_gene.test(tests_passed, opponent_pieces_targeted_board, Piece_Color::WHITE, opponent_pieces_targeted_score);
    }

    void total_force_gene_tests(bool& tests_passed)
    {
        auto piece_strength_gene = Piece_Strength_Gene();
        piece_strength_gene.read_from("testing/test_genome.txt");
        const auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
        total_force_gene.test(tests_passed, Board(), Piece_Color::WHITE, 1.0);
    }

    void sphere_of_influence_gene_tests(bool& tests_passed)
    {
        auto sphere_of_influence_gene = Sphere_of_Influence_Gene();
        sphere_of_influence_gene.read_from("testing/test_genome.txt");
        const auto sphere_of_influence_board = Board("k7/8/8/8/1R3p2/8/8/K7 w - - 0 1");
        const auto sphere_of_influence_score =
              ((4.0 * (1 + (2.0/(1 + 1.0))))  // b8
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
             + (1.0 * (1 + (2.0/(1 + 7.0)))))  // h4
            /(64*(4.0 + 1.0));
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
    }

    void pawn_structure_gene_tests(bool& tests_passed)
    {
        auto pawn_structure_gene = Pawn_Structure_Gene();
        pawn_structure_gene.read_from("testing/test_genome.txt");

        // Pawn protected by pawn
        const auto board1 = Board("k7/4p3/8/8/8/4P3/3P4/K7 w - - 0 1");
        pawn_structure_gene.test(tests_passed, board1, Piece_Color::WHITE, 0.6/8);
        pawn_structure_gene.test(tests_passed, board1, Piece_Color::BLACK, 0.0/8);

        // Black pawn protected by piece, white pawn protected by pawn in one normal move
        const auto board2 = Board("k7/r1p5/8/8/1P6/8/P7/7K w - - 0 1");
        pawn_structure_gene.test(tests_passed, board2, Piece_Color::WHITE, 0.0/8);
        pawn_structure_gene.test(tests_passed, board2, Piece_Color::BLACK, 0.4/8);

        // Black pawn protected by piece, white pawn protected by pawn in one double move
        const auto board3 = Board("k7/r1p5/8/1P6/8/8/P7/7K w - - 0 1");
        pawn_structure_gene.test(tests_passed, board3, Piece_Color::WHITE, 0.0/8);
        pawn_structure_gene.test(tests_passed, board3, Piece_Color::BLACK, 0.4/8);

        // Black pawn protected by piece, white pawn not protected
        const auto board4 = Board("k7/r1p5/1P6/8/8/P7/8/7K w - - 0 1");
        pawn_structure_gene.test(tests_passed, board4, Piece_Color::WHITE, 0.0/8);
        pawn_structure_gene.test(tests_passed, board4, Piece_Color::BLACK, 0.4/8);
    }

    void game_progress_on_new_board_is_zero(bool& tests_passed)
    {
        auto piece_strength_gene = Piece_Strength_Gene();
        piece_strength_gene.read_from("testing/test_genome.txt");
        const auto game_progress = piece_strength_gene.game_progress(Board{});
        test_result(tests_passed,
                    std::abs(game_progress) < 1e-8,
                    "Game progress at beginning of game is not zero: " + std::to_string(game_progress));
    }

    void game_progress_where_one_side_has_only_king_is_one(bool& tests_passed)
    {
        auto piece_strength_gene = Piece_Strength_Gene();
        piece_strength_gene.read_from("testing/test_genome.txt");
        const auto board = Board("4k3/8/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 0 1");
        const auto game_progress = piece_strength_gene.game_progress(board);
        test_result(tests_passed,
                    std::abs(game_progress - 1.0) < 1e-8,
                    "Game progress with one size having only one king is not one: " + std::to_string(game_progress));
    }

    void game_progress_with_one_queen_removed_makes_sense(bool& tests_passed)
    {
        auto piece_strength_gene = Piece_Strength_Gene();
        piece_strength_gene.read_from("testing/test_genome.txt");
        const auto board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1");
        const auto game_progress = piece_strength_gene.game_progress(board);

        const auto queen_value = std::abs(piece_strength_gene.piece_value({Piece_Color::WHITE, Piece_Type::QUEEN}));
        const auto king_value = std::abs(piece_strength_gene.piece_value({Piece_Color::WHITE, Piece_Type::KING}));
        const auto expected_progress = queen_value/(1.0 - king_value);

        test_result(tests_passed,
                    std::abs(game_progress - expected_progress) < 1e-8,
                    "Game progress with one queen missing not as expected: "
                    + std::to_string(game_progress) + " != " + std::to_string(expected_progress));
    }

    void split_and_join_are_inverse_operations(bool& tests_passed)
    {
        const auto split_join_input = "a/b/c/d";
        const auto splitter = "/";
        const auto split = String::split(split_join_input, splitter);
        const auto rejoin = String::join(split.begin(), split.end(), splitter);
        test_result(tests_passed, split_join_input == rejoin, std::string{"Split-join failed: "} + split_join_input + " --> " + rejoin);
    }

    void commas_as_thousands_separators_correctly_placed(bool& tests_passed)
    {
        const std::vector<std::pair<int, std::string>> tests =
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
    }

    void specified_time_added_to_clock_after_specified_number_of_punches(bool& tests_passed)
    {
        const auto time = Clock::seconds{30};
        Clock::seconds expected_time_after_reset = 2 * time;
        size_t moves_to_reset = 40;
        auto clock = Clock(time, moves_to_reset);
        clock.start(Piece_Color::WHITE);
        for(size_t i = 0; i < 2 * moves_to_reset; ++i)
        {
            const auto pause_start = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(5ms);
            Board timing_board;
            clock.punch(timing_board);
            if(clock.running_for() == Piece_Color::WHITE)
            {
                const auto pause_end = std::chrono::steady_clock::now();
                expected_time_after_reset -= (pause_end - pause_start);
            }
        }
        clock.stop();
        test_result(tests_passed, std::chrono::abs(clock.time_left(Piece_Color::BLACK) - expected_time_after_reset) < 1ms,
                    "Clock reset incorrect: time left for black is " + std::to_string(clock.time_left(Piece_Color::BLACK).count()) + " sec." +
                    " Should be " + std::to_string(expected_time_after_reset.count()) + "sec.");
    }

    void clock_with_increment_gets_time_added_on_every_punch(bool& tests_passed)
    {
        const auto increment = 5s;
        auto clock2 = Clock(Clock::seconds{30}, 0, increment);
        clock2.start(Piece_Color::WHITE);
        auto expected_time = clock2.initial_time();
        for(size_t i = 0; i < 100; ++i)
        {
            auto pause_start = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(5ms);
            Board timing_board;
            clock2.punch(timing_board);
            if(clock2.running_for() == Piece_Color::WHITE)
            {
                auto pause_end = std::chrono::steady_clock::now();
                expected_time += increment - (pause_end - pause_start);
            }
        }
        clock2.stop();
        test_result(tests_passed, std::chrono::abs(clock2.time_left(Piece_Color::BLACK) - expected_time) < 1ms,
                    std::string("Clock increment incorrect: time left for black is ") + std::to_string(clock2.time_left(Piece_Color::BLACK).count()) + " sec." +
                    " Should be " + std::to_string(expected_time.count()) + "sec.");
    }

    void midgame_node_result_values_compare_correctly(bool& tests_passed)
    {
        const Game_Tree_Node_Result r1 = {10,
                                          Piece_Color::WHITE,
                                          {nullptr, nullptr, nullptr}};
        const Game_Tree_Node_Result r2 = {10,
                                          Piece_Color::BLACK,
                                          {nullptr, nullptr, nullptr}};
        test_result(tests_passed, r2.value(Piece_Color::WHITE) < r1.value(Piece_Color::WHITE), "1. Error in comparing Game Tree Node Results.");
        test_result(tests_passed, r1.value(Piece_Color::BLACK) < r2.value(Piece_Color::BLACK), "2. Error in comparing Game Tree Node Results.");
        test_result(tests_passed, r1.value(Piece_Color::WHITE) > r1.value(Piece_Color::BLACK), "1. Error in comparing Game Tree Node Results after color switch.");
        test_result(tests_passed, r2.value(Piece_Color::BLACK) > r2.value(Piece_Color::WHITE), "2. Error in comparing Game Tree Node Results after color switch.");
    }

    void midgame_alpha_beta_values_compare_correctly_with_node_values(bool& tests_passed)
    {
        const Game_Tree_Node_Result r1 = {10,
                                          Piece_Color::WHITE,
                                          {nullptr, nullptr, nullptr}};
        const Game_Tree_Node_Result r2 = {10,
                                          Piece_Color::BLACK,
                                          {nullptr, nullptr, nullptr}};
        const auto abv = r1.alpha_beta_value();
        test_result(tests_passed, abv.value(Piece_Color::WHITE) == r1.value(Piece_Color::WHITE), "1. Incorrect construction of Alpha-Beta Value");
        test_result(tests_passed, abv.value(Piece_Color::BLACK) == r2.value(Piece_Color::WHITE), "2. Incorrect construction of Alpha-Beta Value");
    }

    void alpha_beta_result_values_compare_in_line_with_algorithm(bool& tests_passed)
    {
        const Game_Tree_Node_Result alpha_start = {Game_Tree_Node_Result::lose_score,
                                                   Piece_Color::WHITE,
                                                   {}};

        const Game_Tree_Node_Result beta_start = {Game_Tree_Node_Result::win_score,
                                                  Piece_Color::WHITE,
                                                  {}};
        test_result(tests_passed, alpha_start.value(Piece_Color::WHITE) < beta_start.value(Piece_Color::WHITE), "3. Error in comparing Game Tree Node Results.");
        test_result(tests_passed, alpha_start.value(Piece_Color::BLACK) > beta_start.value(Piece_Color::BLACK), "4. Error in comparing Game Tree Node Results.");
    }

    void alpha_and_beta_value_comparisons_fit_algorithm_definitions(bool& tests_passed)
    {
        const auto alpha_start2 = Alpha_Beta_Value{Game_Tree_Node_Result::lose_score,
                                                   Piece_Color::WHITE,
                                                   0};

        const auto beta_start2 = Alpha_Beta_Value{Game_Tree_Node_Result::win_score,
                                                  Piece_Color::WHITE,
                                                  0};
        test_result(tests_passed, alpha_start2.value(Piece_Color::WHITE) < beta_start2.value(Piece_Color::WHITE), "1. Error in comparing Alpha-Beta Values.");
        test_result(tests_passed, alpha_start2.value(Piece_Color::BLACK) > beta_start2.value(Piece_Color::BLACK), "2. Error in comparing Alpha-Beta Values.");
    }

    void endgame_node_result_tests(bool& tests_passed)
    {
        const Game_Tree_Node_Result white_win4 = {Game_Tree_Node_Result::win_score,
                                                  Piece_Color::WHITE,
                                                  {nullptr, nullptr, nullptr,
                                                   nullptr, nullptr}};
        const Game_Tree_Node_Result white_win6 = {Game_Tree_Node_Result::win_score,
                                                  Piece_Color::WHITE,
                                                  {nullptr, nullptr, nullptr,
                                                   nullptr, nullptr, nullptr,
                                                   nullptr}};
        test_result(tests_passed, white_win6.value(Piece_Color::WHITE) < white_win4.value(Piece_Color::WHITE), "Later win preferred over earlier win.");
        test_result(tests_passed, white_win4.value(Piece_Color::BLACK) < white_win6.value(Piece_Color::BLACK), "Earlier loss preferred over later win.");

        const Game_Tree_Node_Result black_loss6 = {Game_Tree_Node_Result::lose_score,
                                                   Piece_Color::BLACK,
                                                   {nullptr, nullptr, nullptr,
                                                    nullptr, nullptr, nullptr,
                                                    nullptr}};
        test_result(tests_passed, white_win6.value(Piece_Color::WHITE) == black_loss6.value(Piece_Color::WHITE), "1. White win in 6 not equal to black loss in 6.");
        test_result(tests_passed, white_win6.value(Piece_Color::BLACK) == black_loss6.value(Piece_Color::BLACK), "2. White win in 6 not equal to black loss in 6.");

        test_result(tests_passed, black_loss6.is_winning_for(Piece_Color::WHITE), "Black loss in 6 returns false for is_winning_for(Piece_Color::WHITE).");
        test_result(tests_passed, black_loss6.is_losing_for(Piece_Color::BLACK), "Black loss in 6 returns false for is_losing_for(Piece_Color::BLACK).");
    }

    void average_moves_left_matches_precalculated_value(bool& tests_passed)
    {
        const double mean_moves = 26.0;
        const double width = .5;
        const size_t moves_so_far = 22;
        const auto moves_left = Math::average_moves_left(mean_moves, width, moves_so_far);
        const auto expected_moves_left = 16.103940;
        test_result(tests_passed, std::abs(moves_left - expected_moves_left) < 1e-4,
                    std::string("Log-Norm failed: Expected: ") + std::to_string(expected_moves_left) +
                    " --- Got: " + std::to_string(moves_left));
    }

    void average_moves_left_returns_finite_result_after_zero_moves(bool& tests_passed)
    {
        const double mean_moves = 26.0;
        const double width = .5;
        const size_t moves_played_at_start = 0;
        const auto moves_left_at_start = Math::average_moves_left(mean_moves, width, moves_played_at_start);
        test_result(tests_passed, std::isfinite(moves_left_at_start),
                    std::string("Log-Norm failed with zero moves: Expected finite answer, Got: ") + std::to_string(moves_left_at_start));
    }

    void average_moves_left_returns_finite_result_after_one_move(bool& tests_passed)
    {
        const double mean_moves = 26.0;
        const double width = .5;
        const size_t moves_played_after_one_move = 1;
        const auto moves_left_after_one_move = Math::average_moves_left(mean_moves, width, moves_played_after_one_move);
        test_result(tests_passed, std::isfinite(moves_left_after_one_move),
                    std::string("Log-Norm failed after one move: Expected finite answer, Got: ") + std::to_string(moves_left_after_one_move));
    }

    void math_normalize_test(bool& tests_passed)
    {
        const auto too_far = [](const auto value, const auto goal)
        {
            return std::abs(value - goal) > 1e-5;
        };

        auto x = 2.0;
        auto y = 8.0;
        const auto expected_x = 0.2;
        const auto expected_y = 0.8;
        Math::normalize(x, y);
        if(too_far(x, expected_x) || too_far(y, expected_y))
        {
            std::cerr << "Normalizing (2.0, 8.0) should have given (" << expected_x << ", " << expected_y << ") .Got(" << x << ", " << y << ").\n";
            tests_passed = false;
        }

        auto a = 2.0;
        auto b = 8.0;
        auto c = -10.0;
        const auto expected_a = 0.1;
        const auto expected_b = 0.4;
        const auto expected_c = -0.5;
        Math::normalize(a, b, c);
        if(too_far(a, expected_a) || too_far(b, expected_b) || too_far(c, expected_c))
        {
            std::cerr << "Normalizing (2.0, 8.0, -10.0) should have given (" << expected_a << ", " << expected_b << ", " << expected_c << "). Got (" << a << ", " << b << ", " << c << ").\n";
            tests_passed = false;
        }
    }

    void scoped_push_back_works_as_advertised(bool& tests_passed)
    {
        using data = std::vector<int>;
        const auto error = [](const data& result, const data& expected)
            {
                const std::string intro = "Got wrong result. Expected: ";
                const std::string but = ", but got: ";

                const auto to_string_array = [](const data& data_list)
                    {
                        std::string result_text = "{";
                        if(data_list.empty())
                        {
                            return result_text + "}";
                        }
                        result_text += std::to_string(data_list.front());
                        std::for_each(std::next(data_list.begin()), data_list.end(),
                                      [&result_text](const auto& n) { result_text += ", " + std::to_string(n); });
                        return result_text + "}";
                    };

                return intro + to_string_array(expected) + but + to_string_array(result);
            };

        data numbers{};

        {
            const auto guard1 = Algorithm::scoped_push_back(numbers, 1);
            const auto expected1 = data{1};
            test_result(tests_passed, numbers == expected1, error(numbers, expected1));

            {
                const data number_range{2, 3, 4, 5};
                const data expected2 = {1, 2, 3, 4, 5};
                const auto guard2 = Algorithm::scoped_push_back(numbers, number_range.begin(), number_range.end());
                test_result(tests_passed, numbers == expected2, error(numbers, expected2));
            }

            test_result(tests_passed, numbers == expected1, error(numbers, expected1) + " (after removal)");
        }

        test_result(tests_passed, numbers.empty(), error(numbers, {}) + " (after removal)");
    }

    void has_exactly_n_works_as_advertised(bool& tests_passed)
    {
        std::vector<int> data{};
        const auto max_value = 5;
        for(auto value = 1; value <= max_value; ++value)
        {
            for(auto reps = 1; reps <= value; ++reps)
            {
                data.push_back(value);
            }
        }

        // data == {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, ...}

        for(auto value = 1; value <= max_value; ++value)
        {
            for(auto reps = 1; reps <= max_value; ++reps)
            {
                const auto result = Algorithm::has_exactly_n(data.begin(),
                                                             data.end(),
                                                             [value](const auto item) { return item == value; },
                                                             reps);
                test_result(tests_passed,
                            result == (value == reps),
                            std::string{"has_exactly_n failed for "} + std::to_string(reps) + " copies of " + std::to_string(value));
            }
        }
    }
}
