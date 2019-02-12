#include "Testing.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#include <thread>
#include <iomanip>
#include <cassert>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"
#include "Game/Piece.h"

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

#include "Utility.h"

#include "Exceptions/Illegal_Move.h"

// Declaration to silence warnings
bool files_are_identical(const std::string& file_name1, const std::string& file_name2);
size_t move_count(const Board& board, size_t maximum_depth, const std::string& line_prefix);

bool run_tests()
{
    bool tests_passed = true;

    // Basic chess rules check
    Board starting_board;
    while(true)
    {
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
    second_move_board.submit_move(second_move_board.create_move("e4"));
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
                board.create_move('e', 1, final_file, 1);
            }
            catch(const Illegal_Move&)
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
        white_pawn_board.create_move('d', 4, 'e', 5);
    }
    catch(const Illegal_Move&)
    {
        white_pawn_board.ascii_draw(WHITE);
        std::cerr << "Number of legal moves = " << white_pawn_board.legal_moves().size() << std::endl;
        std::cerr << "Pawn capture by white should be legal." << std::endl;
        tests_passed = false;
    }

    Board black_pawn_board("k7/8/8/4p3/3P4/8/8/K7 b - - 0 1");
    try
    {
        black_pawn_board.create_move('e', 5, 'd', 4);
    }
    catch(const Illegal_Move&)
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
    auto side_effects_board = Board("2k5/P7/K7/8/8/8/8/8 w - - 0 1");
    side_effects_board.submit_move(side_effects_board.create_move("a8=Q"));
    std::string bad_move = "Kb8";
    auto illegal_move_made = true;
    try
    {
        side_effects_board.create_move(bad_move);
    }
    catch(const Illegal_Move&)
    {
        illegal_move_made = false;
    }

    if(illegal_move_made)
    {
        side_effects_board.ascii_draw(WHITE);
        std::cerr << bad_move << " should not be legal here." << std::endl;
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
        en_passant_pin_board.create_move(move_string);
    }
    catch(const Illegal_Move&)
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
            perf_board.submit_move(perf_board.create_move(move));
        }
    }
    catch(const Illegal_Move&)
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
        perf_board.submit_move(perf_board.create_move(illegal_move));
    }
    catch(const Illegal_Move&)
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
            perf_board2.submit_move(perf_board2.create_move(move));
        }
    }
    catch(const Illegal_Move&)
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
            perf_board3.submit_move(perf_board3.create_move(move));
        }
    }
    catch(const Illegal_Move&)
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
            result = perf_board4.submit_move(perf_board4.create_move(move));
        }

        if(result.game_has_ended())
        {
            perf_board4.ascii_draw(WHITE);
            std::cerr << "This is not " << result.ending_reason() << "." << std::endl;
            tests_passed = false;
        }
    }
    catch(const Illegal_Move&)
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

    Board perf_board5("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    std::vector<std::string> moves5 = {"Kf1", "Nc4", "Bd1"};
    try
    {
        Game_Result result;
        for(const auto& move : moves5)
        {
            result = perf_board5.submit_move(perf_board5.create_move(move));
        }

        if(result.game_has_ended())
        {
            perf_board5.ascii_draw(WHITE);
            std::cerr << "This is not " << result.ending_reason() << "." << std::endl;
            tests_passed = false;
        }
    }
    catch(const Illegal_Move&)
    {
        perf_board5.ascii_draw(WHITE);
        perf_board5.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
        for(const auto& move : moves5)
        {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        std::cerr << "All moves so far should have been legal." << std::endl;
        tests_passed = false;
    }


    Board perf_board6("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1");
    std::vector<std::string> moves6 = {"Ka6", "Kf1", "h1=B", "a8=Q", "Bxa8"};
    try
    {
        Game_Result result;
        for(const auto& move : moves6)
        {
            result = perf_board6.submit_move(perf_board6.create_move(move));
        }
    }
    catch(const Illegal_Move&)
    {
        perf_board6.ascii_draw(WHITE);
        perf_board6.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
        for(const auto& move : moves6)
        {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        std::cerr << "All moves so far should have been legal." << std::endl;
        tests_passed = false;
    }

    Board perf_board7("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1");
    std::vector<std::string> moves7 = {"bxa8=Q", "Nd6", "Qb7", "gxh1=Q", "c8=Q"};
    try
    {
        Game_Result result;
        for(const auto& move : moves7)
        {
            result = perf_board7.submit_move(perf_board7.create_move(move));
        }
        if(result.winner() != WHITE)
        {
            perf_board7.ascii_draw(WHITE);
            perf_board7.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
            std::cerr << "This should be checkmate for white." << std::endl;
            std::cerr << "Allegedly legal moves:\n";
            for(auto move : perf_board7.legal_moves())
            {
                std::cerr << move->game_record_item(perf_board7) << std::endl;
            }
            tests_passed = false;
        }
    }
    catch(const Illegal_Move&)
    {
        perf_board7.ascii_draw(WHITE);
        perf_board7.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, Clock{});
        for(const auto& move : moves7)
        {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        std::cerr << "All moves so far should have been legal." << std::endl;
        tests_passed = false;
    }

    Board perf_board8("8/6k1/4b3/8/3N2p1/8/6q1/4n1K1 w - - 10 120");
    if(perf_board8.legal_moves().size() > 0)
    {
        perf_board8.ascii_draw(WHITE);
        std::cerr << "This should be checkmate for black." << std::endl;
        std::cerr << "Allegedly legal moves:\n";
        for(auto move : perf_board8.legal_moves())
        {
            std::cerr << move->game_record_item(perf_board8) << std::endl;
        }
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

            const auto fifty_move_board_view = fifty_move_board;
            if(fifty_move_board_view.piece_on_square(move->start_file(), move->start_rank())->type() == PAWN)
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
        ai.set_origin_pool(Random::random_integer(0, 9));
        ai.mutate(1000);
    }
    for(auto& ai : test_pool)
    {
        ai = Genetic_AI(ai, test_pool[Random::random_integer(0, 9)]); // test ancestry writing/parsing
        ai.print(pool_file_name);
    }

    auto index = Random::random_integer(0, int(test_pool.size()) - 1);
    const auto& test_ai = test_pool[index];
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
    std::cout << "Done." << std::endl;


    // Test individual board-scoring genes
    auto test_genes_file_name = "testing/test_genome.txt";

    auto castling_possible_gene = Castling_Possible_Gene();
    castling_possible_gene.read_from(test_genes_file_name);
    auto castling_board = Board("rn2k3/8/8/8/8/8/8/R3K2R w KQq - 0 1");
    auto white_castling_score = 0.8*(4.0/5.0) + 0.2*(5.0/6.0); // maximum score with and without actually castling
    tests_passed &= castling_possible_gene.test(castling_board, white_castling_score);

    castling_board.submit_move(castling_board.create_move("O-O"));
    castling_board.set_turn(WHITE);
    tests_passed &= castling_possible_gene.test(castling_board, 1.0); // full score for kingside castling

    castling_board.set_turn(BLACK);
    auto black_castling_score = 0.2*(3.0/6.0); // castling possible
    tests_passed &= castling_possible_gene.test(castling_board, black_castling_score);

    castling_board.submit_move(castling_board.create_move("Nc6"));
    tests_passed &= castling_possible_gene.test(castling_board, 0.0); // castling no longer relevant

    auto freedom_to_move_gene = Freedom_To_Move_Gene();
    auto freedom_to_move_board = Board("5k2/8/8/8/4Q3/8/8/3K4 w - - 0 1");
    auto freedom_to_move_score = 32.0/20.0;
    tests_passed &= freedom_to_move_gene.test(freedom_to_move_board, freedom_to_move_score);

    auto king_confinement_gene = King_Confinement_Gene();
    king_confinement_gene.read_from(test_genes_file_name);
    auto king_confinement_board = Board("k3r3/8/8/8/8/8/5PPP/7K w - - 0 1");
    auto king_confinement_score = (4*(1.0 + 1.0 + 1.0) + // blocked by friendlies (h2, g2, f2)
                                   (-1)*(1.0 + 1.0))/ // blocked by enemy (e1, e2)
                                   (4 + 1)/ // normalizing
                                   (1.0 + 1.0 + 1.0); // free squares (h1, g1, f1)
    tests_passed &= king_confinement_gene.test(king_confinement_board, king_confinement_score);

    auto king_confined_by_pawns_board = Board("k7/8/8/8/8/pppppppp/8/K7 w - - 0 1");
    auto king_confined_by_pawns_score = (4*(0.0) + // no friendly blockers
                                         (-1)*(8.0))/ // blocked by pawn attacks on second rank
                                         (4 + 1)/ // normalizing
                                         (8.0); // free squares (a1-h1)
    tests_passed &= king_confinement_gene.test(king_confined_by_pawns_board, king_confined_by_pawns_score);

    auto king_protection_gene = King_Protection_Gene();
    auto king_protection_board = king_confinement_board;
    auto max_square_count = 8 + 7 + 7 + 7 + 6; // max_square_count in King_Protection_Gene.cpp
    auto square_count = 7 + 1; // row attack along rank 1 + knight attack from g3
    auto king_protection_score = double(max_square_count - square_count)/max_square_count;
    tests_passed &= king_protection_gene.test(king_protection_board, king_protection_score);

    auto piece_strength_gene = Piece_Strength_Gene();
    piece_strength_gene.read_from(test_genes_file_name);
    auto piece_strength_normalizer = double(32 + 16 + 2*8 + 2*4 + 2*2 + 8*1);

    auto opponent_pieces_targeted_gene = Opponent_Pieces_Targeted_Gene(&piece_strength_gene);
    auto opponent_pieces_targeted_board = Board("k1K5/8/8/8/8/1rp5/nQb5/1q6 w - - 0 1");
    auto opponent_pieces_targeted_score = (16 + 8 + 4 + 2 + 1)/piece_strength_normalizer;
    tests_passed &= opponent_pieces_targeted_gene.test(opponent_pieces_targeted_board, opponent_pieces_targeted_score);

    auto pawn_advancement_gene = Pawn_Advancement_Gene();
    pawn_advancement_gene.read_from(test_genes_file_name);
    auto pawn_advancement_board = Board("7k/4P3/3P4/2P5/1P6/P7/8/K7 w - - 0 1");
    auto pawn_advancement_score = (std::pow(1, 1.2) + std::pow(2, 1.2) + std::pow(3, 1.2) + std::pow(4, 1.2) + std::pow(5, 1.2))/(8*std::pow(5, 1.2));
    tests_passed &= pawn_advancement_gene.test(pawn_advancement_board, pawn_advancement_score);

    auto passed_pawn_gene = Passed_Pawn_Gene();
    auto passed_pawn_board = Board("k1K5/8/8/3pP3/3P4/8/8/8 w - - 0 1");
    auto passed_pawn_score = (1.0 + 2.0/3.0)/8;
    tests_passed &= passed_pawn_gene.test(passed_pawn_board, passed_pawn_score);

    passed_pawn_board.submit_move(passed_pawn_board.create_move("Kd8"));
    passed_pawn_score = (2.0/3.0)/8;
    tests_passed &= passed_pawn_gene.test(passed_pawn_board, passed_pawn_score);

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
    tests_passed &= sphere_of_influence_gene.test(sphere_of_influence_board, sphere_of_influence_score);

    auto total_force_gene = Total_Force_Gene(&piece_strength_gene);
    tests_passed &= total_force_gene.test(Board(), 1.0);

    auto stacked_pawns_gene = Stacked_Pawns_Gene();
    auto stacked_pawns_board = Board("k7/8/8/8/P7/PP6/PPP5/K7 w - - 0 1");
    tests_passed &= stacked_pawns_gene.test(stacked_pawns_board, -3.0/6);

    auto pawn_islands_gene = Pawn_Islands_Gene();
    auto pawn_islands_board = Board("k7/8/8/8/8/8/P1PPP1PP/K7 w - - 0 1");
    tests_passed &= pawn_islands_gene.test(pawn_islands_board, (6.0/3)/8);

    auto checkmate_material_gene = Checkmate_Material_Gene();
    auto checkmate_material_board = Board("k7/8/8/8/8/8/8/6RK w - - 0 1");
    tests_passed &= checkmate_material_gene.test(checkmate_material_board, 1.0); // white can checkmate
    checkmate_material_board.submit_move(*checkmate_material_board.legal_moves().front());
    tests_passed &= checkmate_material_gene.test(checkmate_material_board, 0.0); // black cannot



    // String utilities
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
    catch(const std::runtime_error&)
    {
        // This test should throw an exception.
    }

    try
    {
        original = "   a        b    {    c      d     ";
        auto bad_input_result = String::strip_block_comment(original, "{", "}");
        tests_passed = false;
    }
    catch(const std::runtime_error&)
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

    auto small_number = 1.23456;
    auto small_no_padding = String::round_to_decimal(small_number, 3);
    if(small_no_padding != "1.234")
    {
        std::cerr << "Rounding small number with no zero-padding failed." << std::endl;
        tests_passed = false;
    }

    auto small_padding = String::round_to_decimal(small_number, 7);
    if(small_padding != "1.2345600")
    {
        std::cerr << "Rounding small number with zero-padding failed." << std::endl;
        tests_passed = false;
    }

    auto large_number = 1.23456e128;
    auto large_no_padding = String::round_to_decimal(large_number, 3);
    if(large_no_padding != "1.234e+128")
    {
        std::cerr << "Rounding large number with no zero-padding failed." << std::endl;
        tests_passed = false;
    }

    auto large_padding = String::round_to_decimal(large_number, 7);
    if(large_padding != "1.2345600e+128")
    {
        std::cerr << "Rounding large number with zero-padding failed." << std::endl;
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
    if(std::abs(clock2.time_left(BLACK) - expected_time) > 0.2)
    {
        std::cerr << "Clock increment incorrect: time left for black is " << clock2.time_left(BLACK) << " sec. Should be " << expected_time << "sec." << std::endl;
        tests_passed = false;
    }

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
            board.submit_move(board.create_move(move));
        }
    }
    catch(const Illegal_Move&)
    {
        ambiguous_move_caught = true;
    }

    if( ! ambiguous_move_caught)
    {
        std::cerr << "Last move should have been an error:" << std::endl;
        board.print_game_record(nullptr, nullptr,
                                "", Game_Result(NONE, ""), 0, 0, 0, Clock());

        tests_passed = false;
    }

    // Unambiguous move check
    Board unambiguous;
    std::string unambiguous_move = "Ng1h3";
    try
    {
        unambiguous.create_move(unambiguous_move);
    }
    catch(const Illegal_Move&)
    {
        unambiguous.ascii_draw(WHITE);
        std::cerr << "Unambiguous move (" << unambiguous_move << ") deemed illegal." << std::endl;
        tests_passed = false;
    }

    Board en_passant_pin("7k/4p3/8/2KP3r/8/8/8/8 b - - 0 1");
    en_passant_pin.submit_move(en_passant_pin.create_move("e5"));
    try
    {
        en_passant_pin.submit_move(en_passant_pin.create_move("dxe6"));
        en_passant_pin.ascii_draw(WHITE);
        std::cerr << "Last move (" << en_passant_pin.last_move_record() << ") should have been illegal." << std::endl;
        tests_passed = false;
    }
    catch(const Illegal_Move&)
    {
    }

    Board threat_iterator_bug("8/6K1/2k5/5n2/8/8/8/5R2 w - - 0 1");
    auto test_move_text = "Rxf5";
    try
    {
        threat_iterator_bug.create_move(test_move_text);
    }
    catch(const Illegal_Move&)
    {
        threat_iterator_bug.ascii_draw(WHITE);
        std::cerr << test_move_text << " should have been legal." << std::endl;
        tests_passed = false;
    }

    Board en_passant_pin_capture("7R/8/8/8/1K2p2q/8/5P2/3k4 w - - 0 1");
    try
    {
        for(auto move : {"f4", "exf3", "Rxh4"})
        {
            en_passant_pin_capture.submit_move(en_passant_pin_capture.create_move(move));
        }
    }
    catch(const Illegal_Move&)
    {
        en_passant_pin_capture.ascii_draw(WHITE);
        std::cerr << "Capturing the queen with the rook should be possible here." << std::endl;
        tests_passed = false;
    }

    Board castling_double_pin("8/8/5r2/8/8/8/8/R3K1k1 w Q - 0 1");
    try
    {
        for(auto move : {"O-O-O", "Rf1"})
        {
            castling_double_pin.submit_move(castling_double_pin.create_move(move));
        }
    }
    catch(const Illegal_Move&)
    {
        castling_double_pin.ascii_draw(WHITE);
        std::cerr << "Blocking check with rook should be legal here." << std::endl;
        tests_passed = false;
    }

    auto multiple_check_board = Board("Q6r/4k3/8/8/8/5b2/8/7K w - - 0 1");
    auto multi_check_list = multiple_check_board.legal_moves();
    if(multi_check_list.size() != 1 || multi_check_list.front()->coordinate_move() != "h1g1")
    {
        multiple_check_board.ascii_draw(WHITE);
        std::cerr << "Only Kg1 should be legal here." << std::endl;
        std::cerr << "Legal moves found:";
        for(auto move : multi_check_list)
        {
            std::cerr << " " << move->game_record_item(multiple_check_board);
        }
        std::cerr << std::endl;

        tests_passed = false;
    }

    try
    {
        auto too_many_attackers_board = Board("1k1R4/2P5/KR5q/8/8/8/8/8 b - - 0 1");
        too_many_attackers_board.ascii_draw(WHITE);
        std::cerr << "Illegal board created with too many pieces attacking black king." << std::endl;
        tests_passed = false;
    }
    catch(const std::runtime_error&)
    {
    }

    try
    {
        auto too_many_knight_attackers_board = Board("k7/2N5/1N6/8/8/8/8/7K b - - 0 1");
        too_many_knight_attackers_board.ascii_draw(WHITE);
        std::cerr << "Illegal board created with too many knights attacking black king." << std::endl;
        tests_passed = false;
    }
    catch(const std::runtime_error&)
    {
    }


    // En passant check-rescue
    auto en_passant_check = Board("k7/8/8/3pP3/4B3/8/8/7K w - d6 0 1");
    en_passant_check.submit_move(en_passant_check.create_move("exd6"));
    if( ! en_passant_check.king_is_in_check())
    {
        en_passant_check.ascii_draw(WHITE);
        std::cerr << "King should be in check." << std::endl;
        tests_passed = false;
    }

    // En passant discovered check
    auto en_passant_check_capture = Board("k7/8/8/3pP3/2K5/8/8/8 w - d6 0 1");
    try
    {
        en_passant_check_capture.create_move("exd6");
    }
    catch(const std::exception&)
    {
        en_passant_check_capture.ascii_draw(WHITE);
        en_passant_check_capture.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, {});
        std::cerr << "Move exd6 should be legal." << std::endl;
        tests_passed = false;
    }

    // check that square colors are correct
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


    // Enough material to checkmate
    auto only_kings = Board("k1K5/8/8/8/8/8/8/8 w - - 0 1");
    if(only_kings.enough_material_to_checkmate())
    {
        std::cerr << "This board cannot result in checkmate." << std::endl;
        only_kings.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto one_queen = Board("k1KQ4/8/8/8/8/8/8/8 w - - 0 1");
    if( ! one_queen.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        one_queen.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto opposite_color_bishops = Board("k1KBB3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! opposite_color_bishops.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        opposite_color_bishops.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto same_color_bishops = Board("k1KB1B2/8/8/8/8/8/8/8 w - - 0 1");
    if(same_color_bishops.enough_material_to_checkmate())
    {
        std::cerr << "This board cannot result in checkmate." << std::endl;
        same_color_bishops.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto opposite_color_bishops_opposing_sides = Board("k1KBb3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! opposite_color_bishops_opposing_sides.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        opposite_color_bishops_opposing_sides.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto same_color_bishops_opposing_sides = Board("k1KB1b2/8/8/8/8/8/8/8 w - - 0 1");
    if(same_color_bishops_opposing_sides.enough_material_to_checkmate())
    {
        std::cerr << "This board cannot result in checkmate." << std::endl;
        same_color_bishops_opposing_sides.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto two_knights = Board("k1KNn3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! two_knights.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        two_knights.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto two_knights_same_side = Board("k1KNN3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! two_knights_same_side.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        two_knights_same_side.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto knight_bishop_same_side = Board("k1KNB3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! knight_bishop_same_side.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        knight_bishop_same_side.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto knight_bishop_opposing_sides = Board("k1KnB3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! knight_bishop_opposing_sides.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        knight_bishop_opposing_sides.ascii_draw(WHITE);
        tests_passed = false;
    }

    auto knight_bishop_opposing_sides_bishop_first = Board("k1KBn3/8/8/8/8/8/8/8 w - - 0 1");
    if( ! knight_bishop_opposing_sides_bishop_first.enough_material_to_checkmate())
    {
        std::cerr << "This board could result in checkmate." << std::endl;
        knight_bishop_opposing_sides_bishop_first.ascii_draw(WHITE);
        tests_passed = false;
    }

    // Check that pawn-promotion-by-capture doesn't trip Board::ctor() validity rules
    try
    {
        auto pawn_promotion_attack_board = Board("K1k5/3P4/8/8/8/8/8/8 b - - 0 1");
    }
    catch(const std::runtime_error&)
    {
        std::cerr << "Promoting pawn attacks being counted as attacking king multiple times." << std::endl;
        tests_passed = false;
    }



    // Random number generation sample
    int int_width = 10;
    int real_width = 15;
    int norm_width = 15;
    int uint_width = 25;
    std::cout << std::endl;
    std::cout << std::setw(int_width) << "Integers"
              << std::setw(real_width) << "Reals"
              << std::setw(norm_width) << "Laplace"
              << std::setw(uint_width) << "Unsigned Ints" << '\n';
    std::cout << std::setw(int_width) << "+/-1000"
              << std::setw(real_width) << "[0-2]"
              << std::setw(norm_width) << "width = 3"
              << std::setw(uint_width) << "whole range" << '\n';
    for(int i = 0; i < 10; ++i)
    {
        std::cout << std::setw(int_width) << Random::random_integer(-1000, 1000)
                  << std::setw(real_width) << Random::random_real(0, 2)
                  << std::setw(norm_width) << Random::random_laplace(3)
                  << std::setw(uint_width) << Random::random_unsigned_int64() << '\n';
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
            board_fen.ascii_draw(WHITE);
            tests_passed = false;
        }
    }

    auto standard_starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    auto test_starting_fen = Board().fen_status();
    if(test_starting_fen != standard_starting_fen)
    {
        std::cerr << "Incorrect FEN for starting board.\nExpected: " << standard_starting_fen << "\nGot:      " << test_starting_fen << "\n\n";
        tests_passed = false;
    }

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
        capture_board.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, {});
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
        capture_board.print_game_record(nullptr, nullptr, "", {}, 0, 0, 0, {});
        std::cerr << "The previous move did not capture." << std::endl;
        tests_passed = false;
    }

    if(tests_passed)
    {
        std::cout << "All pre-perft tests passed." << std::endl;
    }
    else
    {
        std::cout << "Pre-perft tests failed." << std::endl;
        std::cout << "Press enter to continue with perft test ..." << std::endl;
        std::cin.get();
    }

    // Count game tree leaves (perft) to given depth to validate move generation
    // (downloaded from http://www.rocechess.ch/perft.html)
    // (leaves from starting positions also found at https://oeis.org/A048987)
    size_t max_perft_depth = 6;
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
    auto perft_timer = Scoped_Stopwatch("");
    for(const auto& line : lines)
    {
        auto perft_test_passed = true;
        auto line_parts = String::split(line, " ;");
        auto fen = line_parts.front();
        std::cout << std::endl << '[' << ++test_number << '/' << lines.size() << "] " << fen << std::endl;
        auto perft_board = Board(fen);
        auto tests = std::vector<std::string>(line_parts.begin() + 1, line_parts.end());
        for(const auto& test : tests)
        {
            auto depth_leaves = String::split(test);
            assert(depth_leaves.size() == 2);
            assert(depth_leaves.front().front() == 'D');
            auto depth = std::stoul(depth_leaves.front().substr(1));
            auto prefix = "Depth " + std::to_string(depth) + ": ";
            if(depth > max_perft_depth)
            {
                std::cout << prefix << "skipped" << std::endl;
                continue;
            }
            auto expected_leaves = std::stoul(depth_leaves.back());
            auto leaf_count = move_count(perft_board, depth, prefix);
            if(leaf_count != expected_leaves)
            {
                std::cerr << " Expected: " << expected_leaves << ", Got: " << leaf_count << std::endl;
                perft_test_passed = false;
                tests_passed = false;
                break;
            }
            else
            {
                std::cout << "OK!" << std::endl;
            }
        }

        if( ! perft_test_passed)
        {
            break;
        }
    }

    std::cout << "Perft time: " << perft_timer.time_so_far() << std::endl;


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

size_t move_count(const Board& board, size_t maximum_depth, const std::string& line_prefix)
{
    if(maximum_depth == 0)
    {
        return 1;
    }

    if(maximum_depth == 1)
    {
        if(board.game_record().empty())
        {
            std::cout << line_prefix;
        }
        return board.legal_moves().size();
    }

    size_t count = 0;
    auto first_move_count = board.legal_moves().size();
    auto current_count = 0;
    auto total_squares = 20;
    for(auto move : board.legal_moves())
    {
        if(board.game_record().empty())
        {
            std::cout << '\r' << line_prefix << '[';
            ++current_count;
            auto squares_to_draw = (total_squares*current_count)/first_move_count;
            std::cout << std::string(squares_to_draw, '#');
            std::cout << std::string(total_squares - squares_to_draw, ' ');
            std::cout << "] " << std::flush;
        }
        auto next_board = board;
        next_board.submit_move(*move);
        count += move_count(next_board, maximum_depth - 1, line_prefix);
    }

    return count;
}
