#include "Players/Minimax_AI.h"

#include <iostream>
#include <string>
#include <vector>
#include <array>

#include "Players/Game_Tree_Node_Result.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility/String.h"
#include "Utility/Random.h"
#include "Utility/Fixed_Capacity_Vector.h"

const Move& Minimax_AI::choose_move(const Board& board, const Clock& clock) const noexcept
{
    // Erase data from previous board when starting new game
    if(board.game_length() <= 1)
    {
        reset();
    }

    while(commentary.size() < board.game_length()/2)
    {
        commentary.push_back({});
    }

    nodes_searched = 0;
    clock_start_time = clock.running_time_left();
    maximum_depth = 0;

    nodes_evaluated = 0;
    total_evaluation_time = 0.0;
    time_at_last_output = clock.running_time_left();

    auto real_prior_result = depth_one_results[board.last_move()];
    auto principal_variation = real_prior_result.variation;
    if( ! commentary.empty() && commentary.back().first.variation != principal_variation)
    {
        commentary.back().second = real_prior_result;
    }

    const auto& legal_moves = board.legal_moves();
    if(legal_moves.size() == 1)
    {
        if(principal_variation.size() > 3)
        {
            // search_game_tree() assumes the principal variation starts
            // with the previous move of this player. If a move was forced,
            // then the principal variation needs to be updated to start with
            // the next move of this side after checking that the immediately
            // preceding move was the expected one.
            principal_variation.erase(principal_variation.begin(),
                                      principal_variation.begin() + 2);
        }
        else
        {
            principal_variation.clear();
        }

        depth_one_results.clear();
        commentary.push_back({real_prior_result, {}});
        commentary.back().first.variation = principal_variation;

        if(principal_variation.size() > 2)
        {
            depth_one_results[principal_variation[1]].variation = principal_variation;
        }

        return *legal_moves.front(); // If there's only one legal move, take it.
    }

    auto time_to_use = time_to_examine(board, clock);

    // alpha = highest score found that opponent will allow
    Game_Tree_Node_Result alpha_start = {Game_Tree_Node_Result::lose_score,
                                         board.whose_turn(),
                                         {}};

    // beta = score that will cause opponent to choose a different prior move
    Game_Tree_Node_Result beta_start = {Game_Tree_Node_Result::win_score,
                                        board.whose_turn(),
                                        {}};

    current_variation_store current_variation;

    auto result = search_game_tree(board,
                                   time_to_use,
                                   clock,
                                   board.game_length(),
                                   alpha_start,
                                   beta_start,
                                   principal_variation,
                                   current_variation);

    if(board.thinking_mode() == Thinking_Output_Type::CECP)
    {
        output_thinking_cecp(result, clock, board.whose_turn());
    }
    else if(board.thinking_mode() == Thinking_Output_Type::UCI)
    {
        output_thinking_uci(result, clock, board.whose_turn());
    }

    commentary.push_back({result, {}});
    depth_one_results = depth_two_results[result.variation.front()];
    depth_two_results.clear();

    evaluation_speed = nodes_evaluated/total_evaluation_time;

    return *result.variation.front();
}

const Move* Minimax_AI::expected_response() const noexcept
{
    if( ! commentary.empty() && commentary.back().first.variation.size() > 1)
    {
        return commentary.back().first.variation.at(1);
    }
    else
    {
        return nullptr;
    }
}

Game_Tree_Node_Result Minimax_AI::search_game_tree(const Board& board,
                                                   const double time_to_examine,
                                                   const Clock& clock,
                                                   const size_t prior_real_moves,
                                                   Game_Tree_Node_Result alpha,
                                                   const Game_Tree_Node_Result& beta,
                                                   std::vector<const Move*>& principal_variation,
                                                   current_variation_store& current_variation) const noexcept
{
    const auto time_start = clock.running_time_left();
    const auto depth = board.game_length() - prior_real_moves + 1;
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
    auto moves_left = all_legal_moves.size();

    Game_Tree_Node_Result best_result = {Game_Tree_Node_Result::lose_score,
                                         perspective,
                                         {current_variation.empty() ? all_legal_moves.front() : current_variation.front()}};

    // Pre-loop time to assign to each move for more accurate speed calculations
    auto setup_time_per_move = (time_start - clock.running_time_left())/moves_left;

    for(const auto& move : all_legal_moves)
    {
        auto evaluate_start_time = clock.running_time_left();
        ++nodes_searched;

        class push_guard
        {
            public:
                push_guard(current_variation_store& list, const Move* move) noexcept : push_list(list) { push_list.push_back(move); }
                ~push_guard() noexcept { push_list.pop_back(); }
            private:
                current_variation_store& push_list;
        };
        auto variation_guard = push_guard(current_variation, move);
        assert(current_variation.size() == depth);

        auto next_board = board;

        auto move_result = next_board.submit_move(*move);
        if(move_result.winner() != NONE)
        {
            // This move results in checkmate, no other move can be better.
            return create_result(next_board, perspective, move_result, prior_real_moves, current_variation.to_vector());
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

        double time_left = time_to_examine - (time_start - clock.running_time_left());
        double time_allotted_for_this_move = (time_left/moves_left)*speculation_time_factor();
        time_allotted_for_this_move = std::min(time_allotted_for_this_move, clock.running_time_left());

        bool recurse;
        if(move_result.game_has_ended())
        {
            recurse = false;
        }
        else if(current_variation.full())
        {
            recurse = false; // prevent stack overflow
        }
        else if( ! principal_variation.empty())
        {
            recurse = true;
        }
        else
        {
            auto minimum_time_to_recurse = next_board.legal_moves().size()/evaluation_speed;
            recurse = (time_allotted_for_this_move > minimum_time_to_recurse);
        }

        Game_Tree_Node_Result result;
        if(recurse)
        {
            result = search_game_tree(next_board,
                                      time_allotted_for_this_move,
                                      clock,
                                      prior_real_moves,
                                      beta,
                                      alpha,
                                      principal_variation,
                                      current_variation);
        }
        else
        {
            auto quiescent_moves = next_board.quiescent(piece_values());
            for(auto quiescent_move : quiescent_moves)
            {
                next_board.submit_move(*quiescent_move);
            }
            auto variation = current_variation.to_vector();
            variation.insert(variation.end(), quiescent_moves.begin(), quiescent_moves.end());
            result = create_result(next_board, perspective, move_result, prior_real_moves, variation);
            nodes_searched += result.depth() - depth;
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
                else if(time_since_last_output(clock) > 1.0)
                {
                    if(board.thinking_mode() == Thinking_Output_Type::CECP)
                    {
                        output_thinking_cecp(alpha, clock,
                                             depth % 2 == 1 ? perspective : opposite(perspective));
                    }
                    else if(board.thinking_mode() == Thinking_Output_Type::UCI)
                    {
                        output_thinking_uci(alpha, clock,
                                            depth % 2 == 1 ? perspective : opposite(perspective));
                    }
                    time_at_last_output = clock.running_time_left();
                }
            }
        }

        --moves_left;
        principal_variation.clear(); // only the first move is part of the principal variation

        if( ! recurse) // This move was scored by genome.evaluate().
        {
            ++nodes_evaluated;
            total_evaluation_time += setup_time_per_move + (evaluate_start_time - clock.running_time_left());
        }

        if(clock.running_time_left() < 0 || board.must_pick_move_now())
        {
            break;
        }
    }

    if(depth == 3 && best_result.variation.size() > 2)
    {
        depth_two_results[best_result.variation[0]][best_result.variation[1]] = best_result;
    }

    return best_result;
}

void Minimax_AI::output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                      const Clock& clock,
                                      Color perspective) const noexcept
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

    auto time_so_far = clock_start_time - clock.running_time_left();
    std::cout << thought.depth() // ply
        << " "
        << int(score) // score in what should be centipawns
        << " "
        << int(time_so_far*100) // search time in centiseconds
        << " "
        << nodes_searched
        << " ";

    if( ! use_short_post())
    {
        std::cout << maximum_depth
            << " "
            << int(nodes_searched/time_so_far)
            << '\t';
    }

    // Principal variation
    for(const auto& move : thought.variation)
    {
        std::cout << move->coordinate_move() << ' ';
    }

    std::cout << std::endl;
}

void Minimax_AI::output_thinking_uci(const Game_Tree_Node_Result& thought, const Clock& clock, Color perspective) const noexcept
{
    auto time_so_far = clock_start_time - clock.running_time_left();
    std::cout << "info"
              << " depth " << thought.depth()
              << " time " << int(time_so_far*1000)
              << " nodes " << nodes_searched
              << " nps " << int(nodes_searched/time_so_far)
              << " pv ";
    for(const auto& move : thought.variation)
    {
        std::cout << move->coordinate_move() << " ";
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
    std::cout << " currmove " << thought.variation.front()->coordinate_move();
    std::cout << std::endl;
}

double Minimax_AI::time_since_last_output(const Clock& clock) const noexcept
{
    return time_at_last_output - clock.running_time_left();
}

Game_Tree_Node_Result Minimax_AI::create_result(Board board,
                                                Color perspective,
                                                const Game_Result& move_result,
                                                size_t prior_real_moves,
                                                const std::vector<const Move*>& move_list) const noexcept
{
    return {evaluate(board, move_result, perspective, prior_real_moves),
            perspective,
            move_list};
}

void Minimax_AI::calibrate_thinking_speed() const noexcept
{
    evaluation_speed = 100; // very conservative initial guess
    auto calibration_time = 1.0; // seconds
    Board board;
    Clock clock(calibration_time, 1, 0.0);
    clock.start();
    choose_move(board, clock);
}

double Minimax_AI::evaluate(const Board& board, const Game_Result& move_result, Color perspective, size_t prior_real_moves) const noexcept
{
    if(move_result.game_has_ended())
    {
        if(move_result.winner() == NONE) // stalemate
        {
            return 0;
        }
        else if(move_result.winner() == perspective) // checkmate win
        {
            return Game_Tree_Node_Result::win_score;
        }
        else // checkmate loss
        {
            return Game_Tree_Node_Result::lose_score;
        }
    }

    return internal_evaluate(board, perspective, prior_real_moves);
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
            auto original_board_result = evaluate(board, {}, WHITE, 0);
            auto minus_pawn_result = evaluate(board_without_pawn, {}, WHITE, 0);
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
    const auto move_label_offset = (board.whose_turn() == WHITE ? 0 : 1);
    std::string result = "(" + (board.whose_turn() == BLACK ? std::to_string(move_number) + ". ... " : std::string{});
    for(size_t i = 0; i < variation.size(); ++i)
    {
        const auto move_label = move_number + i/2 + move_label_offset;
        result += (board.whose_turn() == WHITE ? std::to_string(move_label) + ". " : std::string{}) + variation[i]->game_record_item(board) + " ";
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
        return result + "{" + String::round_to_precision(score, 0.01) + "})";
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
