#include "Players/Minimax_AI.h"

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <chrono>
using namespace std::chrono_literals;
#include <cmath>

#include "Players/Game_Tree_Node_Result.h"
#include "Players/Alpha_Beta_Value.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility/String.h"
#include "Utility/Random.h"
#include "Utility/Fixed_Capacity_Vector.h"
#include "Utility/Math.h"

const Move& Minimax_AI::choose_move(const Board& board, const Clock& clock) const noexcept
{
    // Erase data from previous board when starting new game
    if(board.game_length() <= 1)
    {
        reset();
    }

    while(commentary.size() < board.game_length()/2)
    {
        commentary.emplace_back();
    }

    nodes_searched = 0;
    clock_start_time = std::chrono::steady_clock::now();
    maximum_depth = 0;

    nodes_evaluated = 0;
    total_evaluation_time = 0.0s;
    time_at_last_output = std::chrono::steady_clock::now();

    auto real_prior_result = depth_one_results[board.last_move()];
    auto principal_variation = real_prior_result.variation;
    if( ! commentary.empty() && commentary.back().first.variation != principal_variation)
    {
        commentary.back().second = real_prior_result;
    }

    auto time_to_use = time_to_examine(board, clock);
    auto minimum_search_depth = size_t(std::log(time_to_use/node_evaluation_time)/std::log(branching_factor(game_progress(board))));

    // alpha = highest score found that opponent will allow
    auto alpha_start = Alpha_Beta_Value{Game_Tree_Node_Result::lose_score,
                                        board.whose_turn(),
                                        0};

    // beta = score that will cause opponent to choose a different prior move
    auto beta_start = Alpha_Beta_Value{Game_Tree_Node_Result::win_score,
                                       board.whose_turn(),
                                       0};

    current_variation_store current_variation;

    auto result = search_game_tree(board,
                                   time_to_use,
                                   minimum_search_depth,
                                   clock,
                                   alpha_start,
                                   beta_start,
                                   principal_variation,
                                   current_variation);

    output_thinking(Board::thinking_mode(), result, board.whose_turn());

    commentary.push_back({result, {}});
    depth_one_results = depth_two_results[result.variation.front()];
    depth_two_results.clear();

    if(nodes_evaluated > 0)
    {
        node_evaluation_time = total_evaluation_time/nodes_evaluated;
    }

    return *result.variation.front();
}

Game_Tree_Node_Result Minimax_AI::search_game_tree(const Board& board,
                                                   const Clock::seconds time_to_examine,
                                                   const size_t minimum_search_depth,
                                                   const Clock& clock,
                                                   Alpha_Beta_Value alpha,
                                                   const Alpha_Beta_Value& beta,
                                                   std::vector<const Move*>& principal_variation,
                                                   current_variation_store& current_variation) const noexcept
{
    const auto time_end = std::chrono::steady_clock::now() + time_to_examine;
    const auto depth = current_variation.size() + 1;
    maximum_depth = std::max(maximum_depth, depth);
    auto all_legal_moves = board.legal_moves();

    // The first two items in the principal variation are the last two moves of
    // the non-hypothetical board. So, the first item in the principal variation to
    // consider is at index depth + 1 (since depth starts at 1).
    if(principal_variation.size() > depth + 1)
    {
        auto next_principal_variation_move = principal_variation[depth + 1];
        auto move_iter = std::find(all_legal_moves.begin(),
                                   all_legal_moves.end(),
                                   next_principal_variation_move);

        assert(move_iter != all_legal_moves.end());

        // Put principal variation move at start of list to allow
        // the most pruning later.
        std::iter_swap(all_legal_moves.begin(), move_iter);
    }
    else
    {
        principal_variation.clear();
    }

    // Consider capturing and promoting moves first after principal variation move
    auto partition_start = std::next(all_legal_moves.begin(), principal_variation.empty() ? 0 : 1);
    std::partition(partition_start, all_legal_moves.end(),
                   [&board](auto move){ return board.move_changes_material(*move); });

    const auto perspective = board.whose_turn();
    auto moves_left = all_legal_moves.size() + 1; // + 1 so decrement can go at top of next loop

    Game_Tree_Node_Result best_result = {Game_Tree_Node_Result::lose_score,
                                         perspective,
                                         {current_variation.empty() ? all_legal_moves.front() : current_variation.front()},
                                         false};

    for(const auto& move : all_legal_moves)
    {
        auto evaluate_start_time = std::chrono::steady_clock::now();
        ++nodes_searched;
        --moves_left;

        auto variation_guard = current_variation.scoped_push_back(move);

        auto next_board = board;

        auto move_result = next_board.submit_move(*move);
        if(move_result.winner() != Winner_Color::NONE)
        {
            // This move results in checkmate, no other move can be better.
            best_result = create_result(next_board, perspective, move_result, current_variation);
            break;
        }

        if(alpha.depth() <= depth + 2 && alpha.is_winning_for(perspective))
        {
            // This move will take a longer path to victory
            // than one already found. Use "depth + 2" since,
            // if this move isn't winning (and it isn't, since
            // we're here), then the earliest move that can
            // win is the next one, which is two away (after
            // opponent's move).
            continue;
        }

        auto time_left = Clock::seconds(time_end - std::chrono::steady_clock::now());
        auto time_allotted_for_this_move = (time_left/moves_left)*speculation_time_factor();
        time_allotted_for_this_move = std::min(time_allotted_for_this_move, clock.running_time_left());

        bool recurse;
        if(move_result.game_has_ended())
        {
            recurse = false;
        }
        else if(depth >= maximum_search_depth)
        {
            recurse = false;
        }
        else if(next_board.repeat_count_from_depth(depth) >= 2)
        {
            move_result = Game_Result(Winner_Color::NONE, Game_Result_Type::THREEFOLD_REPETITION);
            recurse = false;
        }
        else if( ! principal_variation.empty())
        {
            recurse = true;
        }
        else if(depth < minimum_search_depth)
        {
            recurse = true;
        }
        else
        {
            auto minimum_time_to_recurse = next_board.legal_moves().size()*node_evaluation_time;
            recurse = (time_allotted_for_this_move > minimum_time_to_recurse);
        }

        Game_Tree_Node_Result result;
        if(recurse)
        {
            result = search_game_tree(next_board,
                                      time_allotted_for_this_move,
                                      minimum_search_depth,
                                      clock,
                                      beta,
                                      alpha,
                                      principal_variation,
                                      current_variation);
        }
        else
        {
            auto quiescent_moves = move_result.game_has_ended() ? std::vector<const Move*>{} : next_board.quiescent(piece_values());
            for(auto quiescent_move : quiescent_moves)
            {
                next_board.submit_move(*quiescent_move);
            }
            auto quiescent_guard = current_variation.scoped_push_back(quiescent_moves.begin(), quiescent_moves.end());
            result = create_result(next_board, perspective, move_result, current_variation);
            nodes_searched += quiescent_moves.size();
        }

        if(result.value(perspective) > best_result.value(perspective))
        {
            best_result = result;
            if(best_result.value(perspective) > alpha.value(perspective))
            {
                alpha = best_result;
                if(alpha.value(perspective) >= beta.value(perspective))
                {
                    break;
                }
                else if(time_since_last_output() > 1s)
                {
                    output_thinking(Board::thinking_mode(),
                                    best_result,
                                    depth % 2 == 1 ? perspective : opposite(perspective));
                    time_at_last_output = std::chrono::steady_clock::now();
                }
            }
        }

        principal_variation.clear(); // only the first move is part of the principal variation

        if( ! recurse)
        {
            ++nodes_evaluated;
            total_evaluation_time += std::chrono::steady_clock::now() - evaluate_start_time;
        }

        if(clock.running_time_left() < 0.0s || Board::must_pick_move_now())
        {
            break;
        }
    }

    if(depth == 3)
    {
        depth_two_results[best_result.variation[0]][best_result.variation[1]] = best_result;
    }

    return best_result;
}

void Minimax_AI::output_thinking(Thinking_Output_Type format,
                                 const Game_Tree_Node_Result& thought,
                                 Piece_Color perspective) const noexcept
{
    if(format == Thinking_Output_Type::CECP)
    {
        output_thinking_cecp(thought, perspective);
    }
    else if(format == Thinking_Output_Type::UCI)
    {
        output_thinking_uci(thought, perspective);
    }
}

void Minimax_AI::output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                      Piece_Color perspective) const noexcept
{
    auto score = thought.corrected_score(perspective)/centipawn_value();

    // Indicate "mate in N moves" where N == thought.depth
    if(thought.is_winning_for(perspective))
    {
        score = 10000.0 - thought.depth();
    }
    else if(thought.is_losing_for(perspective))
    {
        score = -(10000.0 - thought.depth());
    }

    auto time_so_far = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - clock_start_time);
    using centiseconds = std::chrono::duration<int, std::centi>;
    std::cout << thought.depth() // ply
        << " "
        << int(score) // score in what should be centipawns
        << " "
        << std::chrono::duration_cast<centiseconds>(time_so_far).count()
        << " "
        << nodes_searched
        << " "
        << maximum_depth
        << " "
        << int(nodes_searched/time_so_far.count())
        << '\t';

    // Principal variation
    for(const auto& move : thought.variation)
    {
        std::cout << move->coordinates() << ' ';
    }

    std::cout << std::endl;
}

void Minimax_AI::output_thinking_uci(const Game_Tree_Node_Result& thought,
                                     Piece_Color perspective) const noexcept
{
    auto time_so_far = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - clock_start_time);
    std::cout << "info"
              << " depth " << thought.depth()
              << " time " << std::chrono::duration_cast<std::chrono::milliseconds>(time_so_far).count()
              << " nodes " << nodes_searched
              << " nps " << int(nodes_searched/time_so_far.count())
              << " pv ";
    for(const auto& move : thought.variation)
    {
        std::cout << move->coordinates() << " ";
    }
    std::cout << "score ";
    if(thought.is_winning_for(perspective))
    {
        std::cout << "mate " << (1 + thought.depth())/2; // moves, not plies
    }
    else if(thought.is_losing_for(perspective))
    {
        std::cout << "mate -" << (1 + thought.depth())/2; // moves, not plies
    }
    else
    {
        std::cout << "cp " << int(thought.corrected_score(perspective)/centipawn_value());
    }
    std::cout << " currmove " << thought.variation.front()->coordinates();
    std::cout << std::endl;
}

std::chrono::duration<double> Minimax_AI::time_since_last_output() const noexcept
{
    return std::chrono::steady_clock::now() - time_at_last_output;
}

Game_Tree_Node_Result Minimax_AI::create_result(const Board& board,
                                                Piece_Color perspective,
                                                const Game_Result& move_result,
                                                const current_variation_store& move_list) const noexcept
{
    return {evaluate(board, move_result, perspective, move_list.size()),
            perspective,
            {move_list.begin(), move_list.end()},
            move_result.game_has_ended() && move_result.winner() == Winner_Color::NONE};
}

void Minimax_AI::calibrate_thinking_speed() const noexcept
{
    node_evaluation_time = 1ms; // very conservative initial guess
    Board board;
    Clock clock(1s);
    clock.start();
    choose_move(board, clock);
    reset();
}

double Minimax_AI::evaluate(const Board& board, const Game_Result& move_result, Piece_Color perspective, size_t depth) const noexcept
{
    // From the perspective of the player who is ultimately picking the move (the root
    // of the game tree), it doesn't matter who causes the draw. The score of a draw
    // should be the same no matter who causes it.
    const auto draw_score = draw_value()*centipawn_value();

    if(move_result.game_has_ended())
    {
        if(move_result.winner() == Winner_Color::NONE) // draw by rule
        {
            return draw_score;
        }
        else if(move_result.winner() == static_cast<Winner_Color>(perspective)) // checkmate win
        {
            return Game_Tree_Node_Result::win_score;
        }
        else // checkmate loss
        {
            return Game_Tree_Node_Result::lose_score;
        }
    }

    auto score = internal_evaluate(board, perspective, depth);
    auto non_progress_moves = board.moves_since_pawn_or_capture();
    if(non_progress_moves >= depth)
    {
        return Math::interpolate(score, draw_score, non_progress_moves/100.0);
    }
    else
    {
        return score;
    }
}

double Minimax_AI::centipawn_value() const noexcept
{
    return value_of_centipawn;
}

void Minimax_AI::calculate_centipawn_value() const noexcept
{
    auto sum_of_diffs = 0.0;
    auto count = 0;
    while(true)
    {
        Board board;
        auto board_is_good = true;
        for(int move = 0; move < 40; ++move)
        {
            auto next_move = Random::random_element(board.legal_moves());
            if(board.submit_move(*next_move).game_has_ended() || board.fen().find_first_of("pP") == std::string::npos)
            {
                board_is_good = false;
                break;
            }
        }

        if(board_is_good)
        {
            auto board_without_pawn = board.without_random_pawn();
            auto original_board_result = evaluate(board, {}, Piece_Color::WHITE, 0);
            auto minus_pawn_result = evaluate(board_without_pawn, {}, Piece_Color::WHITE, 0);
            auto diff = std::abs(original_board_result - minus_pawn_result);
            sum_of_diffs += diff;

            if((++count > 100 && diff < 0.01*sum_of_diffs) || count > 1000)
            {
                break;
            }
        }
    }

    value_of_centipawn = sum_of_diffs/count/100;
}

std::string Minimax_AI::commentary_for_next_move(const Board& board, size_t move_number) const noexcept
{
    auto comment_index = board.game_length()/2;
    if(comment_index >= commentary.size() || commentary.at(comment_index).first.variation.empty())
    {
        return {};
    }

    const auto& comment = commentary.at(comment_index);
    auto variation = comment.first.variation;
    auto score = comment.first.corrected_score(board.whose_turn())/centipawn_value()/100.0;
    auto alternate_variation = comment.second.variation;
    auto alternate_score = comment.second.corrected_score(board.whose_turn())/centipawn_value()/100.0;
    return variation_line(board, move_number, variation, score, alternate_variation, alternate_score);
}

std::string variation_line(Board board,
                           const size_t move_number,
                           const std::vector<const Move*>& variation,
                           const double score,
                           const std::vector<const Move*>& alternate_variation,
                           const double alternate_score)
{
    Game_Result move_result;
    auto write_alternate_variation = ! alternate_variation.empty();
    const auto move_label_offset = (board.whose_turn() == Piece_Color::WHITE ? 0 : 1);
    std::string result = "(" + (board.whose_turn() == Piece_Color::BLACK ? std::to_string(move_number) + ". ... " : std::string{});
    for(size_t i = 0; i < variation.size(); ++i)
    {
        const auto move_label = move_number + i/2 + move_label_offset;
        result += (board.whose_turn() == Piece_Color::WHITE ? std::to_string(move_label) + ". " : std::string{}) + variation[i]->algebraic(board) + " ";
        if(write_alternate_variation && i < alternate_variation.size() && alternate_variation[i] != variation[i])
        {
            result += variation_line(board,
                                     move_label,
                                     {alternate_variation.begin() + i, alternate_variation.end()},
                                     alternate_score,
                                     {},
                                     {}) + " ";
            write_alternate_variation = false;
        }
        move_result = board.submit_move(*variation[i]);
    }

    if( ! move_result.game_has_ended())
    {
        return result + "{" + String::round_to_decimals(score, 2) + "})";
    }
    else if(variation.size() == 1)
    {
        // No need for commentary on the last move of the game since the result is obvious.
        return {};
    }
    else
    {
        return String::trim_outer_whitespace(result) + ")";
    }
}

void Minimax_AI::undo_move(const Move* last_move) const noexcept
{
    if(commentary.empty())
    {
        return;
    }

    if(commentary.back().first.variation.front() == last_move)
    {
        commentary.pop_back();
    }
}

void Minimax_AI::recalibrate_self() const noexcept
{
    calibrate_thinking_speed();
    calculate_centipawn_value();
}

void Minimax_AI::reset() const noexcept
{
    commentary.clear();
    depth_one_results.clear();
}
