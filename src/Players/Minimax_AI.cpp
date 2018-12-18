#include "Players/Minimax_AI.h"

#include <iostream>

#include "Players/Game_Tree_Node_Result.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility.h"

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

        commentary.push_back(principal_variation);

        return *legal_moves.front(); // If there's only one legal move, take it.
    }

    auto time_to_use = time_to_examine(board, clock);

    // alpha = highest score found that opponent will allow
    Game_Tree_Node_Result alpha_start = {Math::lose_score,
                                         board.whose_turn(),
                                         {}};

    // beta = score that will cause opponent to choose a different prior move
    Game_Tree_Node_Result beta_start = {Math::win_score,
                                        board.whose_turn(),
                                        {}};

    auto result = search_game_tree(board,
                                   time_to_use,
                                   clock,
                                   1,
                                   alpha_start,
                                   beta_start,
                                   ! principal_variation.empty());

    if(board.thinking_mode() == CECP)
    {
        output_thinking_cecp(result, clock, board.whose_turn());
    }

    if(result.depth() > 1)
    {
        commentary.push_back(result.variation);
    }
    else
    {
        commentary.push_back({});
    }

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
        evaluation_speed = nodes_evaluated / total_evaluation_time;
    }

    return *result.variation.front();
}

Game_Tree_Node_Result Minimax_AI::search_game_tree(const Board& board,
                                                   const double time_to_examine,
                                                   const Clock& clock,
                                                   const size_t depth,
                                                   Game_Tree_Node_Result alpha,
                                                   const Game_Tree_Node_Result& beta,
                                                   bool still_on_principal_variation) const
{
    const auto time_start = clock.running_time_left();
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

    // Consider capturing moves first after principal variation move
    auto partition_start = std::next(all_legal_moves.begin(), start_offset);
    std::partition(partition_start, all_legal_moves.end(),
                   [&board](auto move){ return board.move_captures(*move); });

    const auto perspective = board.whose_turn();
    auto moves_left = all_legal_moves.size();

    Game_Tree_Node_Result best_result = {Math::lose_score,
                                         perspective,
                                         {all_legal_moves.front()}};

    for(const auto& move : all_legal_moves)
    {
        auto evaluate_start_time = clock.running_time_left();
        ++nodes_searched;

        auto next_board = board;

        auto move_result = next_board.submit_move(*move);
        if(move_result.winner() != NONE)
        {
            // This move results in checkmate, no other move can be better.
            return create_result(next_board, perspective, move_result, depth);
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
        double time_allotted_for_this_move = (time_left / moves_left)*speculation_time_factor(next_board);
        time_allotted_for_this_move = std::min(time_allotted_for_this_move, clock.running_time_left());

        bool recurse;
        if(move_result.game_has_ended())
        {
            recurse = false;
        }
        else if(still_on_principal_variation)
        {
            recurse = true;
        }
        else if(depth > 300)
        {
            recurse = false; // prevent stack overflow
        }
        else
        {
            auto minimum_time_to_recurse = next_board.legal_moves().size() / evaluation_speed;
            recurse = (time_allotted_for_this_move > minimum_time_to_recurse);
        }

        Game_Tree_Node_Result result;
        if(recurse)
        {
            result = search_game_tree(next_board,
                                      time_allotted_for_this_move,
                                      clock,
                                      depth + 1,
                                      beta,
                                      alpha,
                                      still_on_principal_variation);
        }
        else
        {
            // Record immediate result without looking ahead further
            result = create_result(next_board, perspective, move_result, depth);
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
                else if(board.thinking_mode() == CECP && time_since_last_output(clock) > 0.1)
                {
                    output_thinking_cecp(alpha, clock,
                                         depth % 2 == 1 ? perspective : opposite(perspective));
                    time_at_last_output = clock.running_time_left();
                }
            }
        }

        --moves_left;
        still_on_principal_variation = false; // only the first move is part of the principal variation

        if(clock.running_time_left() < 0)
        {
            break;
        }

        if( ! recurse) // This move was scored by genome.evaluate().
        {
            ++nodes_evaluated;
            total_evaluation_time += evaluate_start_time - clock.running_time_left();
        }
    }

    return best_result;
}

void Minimax_AI::output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                      const Clock& clock,
                                      Color perspective) const
{
    auto score = thought.corrected_score(perspective) / centipawn_value();

    // Indicate "mate in N moves" where N == thought.depth
    if(thought.is_winning_for(perspective))
    {
        score = 10000.0 - thought.depth() + 1;
    }
    else if(thought.is_losing_for(perspective))
    {
        score = -(10000.0 - thought.depth() + 1);
    }

    auto time_so_far = clock_start_time - clock.running_time_left();
    std::cout << thought.depth() // ply
        << " "
        << int(score) // score in what should be centipawns
        << " "
        << int(time_so_far * 100) // search time in centiseconds
        << " "
        << nodes_searched
        << " "
        << maximum_depth
        << " "
        << int(nodes_searched / time_so_far)
        << '\t';

    // Principal variation
    for(const auto& move : thought.variation)
    {
        std::cout << move->coordinate_move() << ' ';
    }

    std::cout << std::endl;
}

double Minimax_AI::time_since_last_output(const Clock& clock) const
{
    return time_at_last_output - clock.running_time_left();
}

Game_Tree_Node_Result Minimax_AI::create_result(const Board& board,
                                                Color perspective,
                                                const Game_Result& move_result,
                                                size_t depth) const
{
    return {evaluate(board, move_result, perspective, depth),
            perspective,
            {board.game_record().end() - depth,
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
    // choose_move() keeps track of the time it takes and the number of positions
    // it sees, so this practice move will update the positions_per_second to a
    // more reasonable value.
}

double Minimax_AI::evaluate(const Board & board, const Game_Result& move_result, Color perspective, size_t depth) const
{
    if(move_result.game_has_ended())
    {
        if(move_result.winner() == NONE) // stalemate
        {
            return 0;
        }
        else if(move_result.winner() == perspective) // checkmate win
        {
            return Math::win_score;
        }
        else // checkmate loss
        {
            return Math::lose_score;
        }
    }

    return internal_evaluate(board, perspective, depth);
}

double Minimax_AI::centipawn_value() const
{
    return value_of_centipawn;
}

void Minimax_AI::calculate_centipawn_value()
{
    auto board_with_pawns = Board("4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
    auto board_with_no_white_pawns = Board("4k3/pppppppp/8/8/8/8/8/4K3 w - - 0 1");
    value_of_centipawn = std::abs(evaluate(board_with_pawns, {}, WHITE, 0) - evaluate(board_with_no_white_pawns, {}, WHITE, 0)) / 800;
}

std::string Minimax_AI::commentary_for_move(size_t move_number) const
{
    std::string result;
    if(move_number < commentary.size() && !commentary.at(move_number).empty())
    {
        result = commentary.at(move_number).front()->coordinate_move();
        for(size_t i = 1; i < commentary.at(move_number).size(); ++i)
        {
            result += " " + commentary.at(move_number).at(i)->coordinate_move();
        }
    }

    return result;
}
