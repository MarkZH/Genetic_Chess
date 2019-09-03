#include "Players/Minimax_AI.h"

#include <iostream>
#include <string>

#include "Players/Game_Tree_Node_Result.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility/String.h"
#include "Utility/Random.h"

const Move& Minimax_AI::choose_move(const Board& board, const Clock& clock) const
{
    // Erase data from previous board when starting new game
    if(board.game_record().size() <= 1)
    {
        principal_variation.clear();
        commentary.clear();
    }

    nodes_searched = 0;
    clock_start_time = clock.running_time_left();
    maximum_depth = 0;

    nodes_evaluated = 0;
    total_evaluation_time = 0.0;
    time_at_last_output = clock.running_time_left();

    const auto& legal_moves = board.legal_moves();
    if(legal_moves.size() == 1)
    {
        if(principal_variation.size() > 3 && principal_variation[1] == board.game_record().back())
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

        commentary.push_back({commentary.empty() ? 0.0 : commentary.back().corrected_score(board.whose_turn()),
                              board.whose_turn(),
                              principal_variation});

        return *legal_moves.front(); // If there's only one legal move, take it.
    }

    // No useful principal variation if it doesn't include the next possible move
    // or if the opponent did not make the predicted next move.
    if(principal_variation.size() < 2 || principal_variation[1] != board.game_record().back())
    {
        principal_variation.clear();
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

    auto result = search_game_tree(board,
                                   time_to_use,
                                   clock,
                                   board.game_record().size(),
                                   alpha_start,
                                   beta_start,
                                   ! principal_variation.empty());

    if(board.thinking_mode() == CECP)
    {
        output_thinking_cecp(result, clock, board.whose_turn());
    }
    else if(board.thinking_mode() == UCI)
    {
        output_thinking_uci(result, clock, board.whose_turn());
    }

    result.time_allotted = time_to_use;
    result.time_used = clock_start_time - clock.running_time_left();

    commentary.push_back(result);

    if(result.depth() > 2)
    {
        principal_variation = result.variation;
    }
    else
    {
        principal_variation.clear();
    }

    if(nodes_evaluated > 0)
    {
        evaluation_speed = nodes_evaluated/total_evaluation_time;
    }

    return *result.variation.front();
}

Game_Tree_Node_Result Minimax_AI::search_game_tree(const Board& board,
                                                   const double time_to_examine,
                                                   const Clock& clock,
                                                   const size_t prior_real_moves,
                                                   Game_Tree_Node_Result alpha,
                                                   const Game_Tree_Node_Result& beta,
                                                   bool still_on_principal_variation) const
{
    const auto time_start = clock.running_time_left();
    const auto depth = board.game_record().size() - prior_real_moves + 1;
    maximum_depth = std::max(maximum_depth, depth);
    auto all_legal_moves = board.legal_moves();

    // The two items in the principal variation are the last two moves of
    // the non-hypothetical board. So, the first item in the principal variation to
    // consider is at index depth + 1 (since depth starts at 1).
    still_on_principal_variation = (still_on_principal_variation && principal_variation.size() > depth + 1);
    auto start_offset = 0;
    if(still_on_principal_variation)
    {
        auto next_principal_variation_move = principal_variation[depth + 1];
        auto move_iter = std::find(all_legal_moves.begin(),
                                   all_legal_moves.end(),
                                   next_principal_variation_move);

        still_on_principal_variation = (move_iter != all_legal_moves.end());
        if(still_on_principal_variation)
        {
            // Put principal variation move at start of list to allow
            // the most pruning later.
            std::iter_swap(all_legal_moves.begin(), move_iter);
            start_offset = 1;
        }
    }

    // Consider capturing and promoting moves first after principal variation move
    auto partition_start = std::next(all_legal_moves.begin(), start_offset);
    std::partition(partition_start, all_legal_moves.end(),
                   [&board](auto move){ return board.move_changes_material(*move); });

    const auto perspective = board.whose_turn();
    auto moves_left = all_legal_moves.size();

    Game_Tree_Node_Result best_result = {Game_Tree_Node_Result::lose_score,
                                         perspective,
                                         {all_legal_moves.front()}};

    // Pre-loop time to assign to each move for more accurate speed calculations
    auto setup_time_per_move = (time_start - clock.running_time_left())/moves_left;

    for(const auto& move : all_legal_moves)
    {
        auto evaluate_start_time = clock.running_time_left();
        ++nodes_searched;

        auto next_board = board;

        auto move_result = next_board.submit_move(*move);
        if(move_result.winner() != NONE)
        {
            // This move results in checkmate, no other move can be better.
            return create_result(next_board, perspective, move_result, prior_real_moves);
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
        double time_allotted_for_this_move = (time_left/moves_left)*speculation_time_factor(next_board);
        time_allotted_for_this_move = std::min(time_allotted_for_this_move, clock.running_time_left());

        bool recurse;
        if(move_result.game_has_ended())
        {
            recurse = false;
        }
        else if(depth > 100)
        {
            recurse = false; // prevent stack overflow
        }
        else if(still_on_principal_variation)
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
                                      still_on_principal_variation);
        }
        else
        {
            result = create_result(next_board.quiescent(piece_values()), perspective, move_result, prior_real_moves);
            nodes_searched += result.depth() - 1;
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
                    if(board.thinking_mode() == CECP)
                    {
                        output_thinking_cecp(alpha, clock,
                                             depth % 2 == 1 ? perspective : opposite(perspective));
                    }
                    else if(board.thinking_mode() == UCI)
                    {
                        output_thinking_uci(alpha, clock,
                                            depth % 2 == 1 ? perspective : opposite(perspective));
                    }
                    time_at_last_output = clock.running_time_left();
                }
            }
        }

        --moves_left;
        still_on_principal_variation = false; // only the first move is part of the principal variation

        if(clock.running_time_left() < 0 || board.must_pick_move_now())
        {
            break;
        }

        if( ! recurse) // This move was scored by genome.evaluate().
        {
            ++nodes_evaluated;
            total_evaluation_time += setup_time_per_move + (evaluate_start_time - clock.running_time_left());
        }
    }

    return best_result;
}

void Minimax_AI::output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                      const Clock& clock,
                                      Color perspective) const
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

void Minimax_AI::output_thinking_uci(const Game_Tree_Node_Result& thought, const Clock& clock, Color perspective) const
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
        std::cout << "mate " << thought.depth()/2; // moves, not plies
    }
    else if(thought.is_losing_for(perspective))
    {
        std::cout << "mate -" << thought.depth()/2; // moves, not plies
    }
    else
    {
        std::cout << "cp " << int(thought.corrected_score(perspective)/centipawn_value());
    }
    std::cout << " currmove " << thought.variation.front()->coordinate_move();
    std::cout << std::endl;
}

double Minimax_AI::time_since_last_output(const Clock& clock) const
{
    return time_at_last_output - clock.running_time_left();
}

Game_Tree_Node_Result Minimax_AI::create_result(const Board& board,
                                                Color perspective,
                                                const Game_Result& move_result,
                                                size_t prior_real_moves) const
{
    return {evaluate(board, move_result, perspective, prior_real_moves),
            perspective,
            {board.game_record().begin() + int(prior_real_moves),
            board.game_record().end()}};
}

void Minimax_AI::calibrate_thinking_speed() const
{
    evaluation_speed = 100; // very conservative initial guess
    auto calibration_time = 1.0; // seconds
    Board board;
    Clock clock(calibration_time, 1, 0.0);
    clock.start();
    choose_move(board, clock);
}

double Minimax_AI::evaluate(const Board& board, const Game_Result& move_result, Color perspective, size_t prior_real_moves) const
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

double Minimax_AI::centipawn_value() const
{
    return value_of_centipawn;
}

void Minimax_AI::calculate_centipawn_value() const
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
            if(board.submit_move(*next_move).game_has_ended() || board.fen_status().find_first_of("pP") == std::string::npos)
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

std::string Minimax_AI::commentary_for_next_move(const Board& board, size_t move_number) const
{
    auto comment_index = board.game_record().size()/2;
    if(comment_index >= commentary.size() || commentary.at(comment_index).variation.empty())
    {
        return {};
    }

    const auto& comment = commentary.at(comment_index);
    Game_Result move_result;
    auto temp_board = board;
    std::string result = "(" + (temp_board.whose_turn() == BLACK ? std::to_string(move_number) + ". ... " : std::string{});
    for(auto move : comment.variation)
    {
        result += (temp_board.whose_turn() == WHITE ? std::to_string(move_number++) + ". " : std::string{}) + move->game_record_item(temp_board) + " ";
        move_result = temp_board.submit_move(*move);
    }

    auto round = [](double x) { return String::round_to_precision(x, 0.01); };
    auto score = round(comment.corrected_score(board.whose_turn()) / centipawn_value() / 100.0);
    auto score_text = "{" + score + ";" + round(comment.time_used) + "/" + round(comment.time_allotted) + "})";

    if( ! move_result.game_has_ended())
    {
        return result + score_text;
    }
    else if(comment.variation.size() == 1)
    {
        // No need for commentary on the last move of the game since the result is obvious.
        return {};
    }
    else if(move_result.winner() == NONE)
    {
        return result + score_text;
    }
    else
    {
        return result + "{" + score_text.substr(score_text.find(';') + 1);
    }
}

void Minimax_AI::recalibrate_self() const
{
    calibrate_thinking_speed();
    calculate_centipawn_value();
}
