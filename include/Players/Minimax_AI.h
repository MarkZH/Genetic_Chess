#ifndef MINIMAX_AI_H
#define MINIMAX_AI_H

#include "Player.h"

#include <vector>
#include <array>

#include "Game/Color.h"
#include "Players/Game_Tree_Node_Result.h"

class Board;
class Clock;
class Move;
class Game_Result;

//! This Player uses a variable-depth minimax algorithm with alpha-beta pruning.
//
//! Minimax algorithm: https://en.wikipedia.org/wiki/Minimax
//! Alpha-beta pruning: https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
class Minimax_AI : public Player
{
    public:
        //! Minimax_AI uses a variable-depth minimax algorithm with alpha-beta pruning.
        //
        //! The depth of the search is determined by how much time is available.
        //! At first, equal time is allocated to each legal move for examination. Time
        //! is overcommitted with the expectation that not all of the time will be used
        //! due to search cutoffs due to alpha-beta pruning.
        //! \param board The current state of the game.
        //! \param clock The game clock telling how much time is left in the game.
        const Move& choose_move(const Board& board, const Clock& clock) const override;

        //! Prints the expected future variation and score for the chosen move.
        //
        //! \param board The state of the game just prior to the move being commented on.
        //! \param move_number The numeric label to use for the move (in case the current
        //!        game did not start with move 1.
        std::string commentary_for_next_move(const Board& board, size_t move_number) const override;

    protected:
        //! Recalculate values that will last the lifetime of the instance.
        //
        //! In this case, the values are an initial estimate of the speed of
        //! searching the game tree and the value of a centipawn for reporting
        //! scores of board positions.
        void recalibrate_self() const;

    private:
        mutable std::vector<const Move*> principal_variation;
        mutable std::vector<Game_Tree_Node_Result> commentary;

        // Monitor search speed to adapt to different computers/competing workloads
        mutable size_t nodes_searched;
        mutable double clock_start_time;
        mutable size_t maximum_depth;

        // For thinking output
        mutable int nodes_evaluated;
        mutable double total_evaluation_time;
        mutable double evaluation_speed;
        mutable double time_at_last_output;

        // Evaluation method
        double evaluate(const Board& board,
                        const Game_Result& move_result,
                        Color perspective,
                        size_t prior_real_moves) const;
        virtual double internal_evaluate(const Board& board,
                                         Color perspective,
                                         size_t prior_real_moves) const = 0;

        virtual const std::array<double, 6>& piece_values() const = 0;

        // Time management
        virtual double time_to_examine(const Board& board, const Clock& clock) const = 0;
        virtual double speculation_time_factor(const Board& board) const = 0;

        // Scoring output
        double centipawn_value() const;

        // Minimax (actually negamax) with alpha-beta pruning
        Game_Tree_Node_Result search_game_tree(const Board& board,
                                               double time_to_examine,
                                               const Clock& clock,
                                               size_t prior_real_moves,
                                               Game_Tree_Node_Result alpha,
                                               const Game_Tree_Node_Result& beta,
                                               bool still_on_principal_variation) const;

        Game_Tree_Node_Result create_result(const Board& board,
                                            Color perspective,
                                            const Game_Result& move_result,
                                            size_t prior_real_moves) const;

        // Output thinking to stdout
        void output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                  const Clock& clock,
                                  Color perspective) const;
        void output_thinking_uci(const Game_Tree_Node_Result& thought,
                                 const Clock& clock,
                                 Color perspective) const;

        double time_since_last_output(const Clock& clock) const;

        mutable double value_of_centipawn;

        //! Approximate the value of 0.01 pawns for reporting scores.
        //
        //! \returns A numerical value to normalize the scores returned by board evaluations
        //!          so that the loss of a random pawn changes the score by about 1.0.
        void calculate_centipawn_value() const;

        //! Initial measurement of evaluation speed of the engine.
        //
        //! The method Minimax_AI::choose_move() keeps track of the time it takes
        //! and the number of positions it evaluates. But, it needs an accurate
        //! starting value for the first move search. So, this practice move will
        //! update the evaluation speed to a more reasonable starting value.
        void calibrate_thinking_speed() const;
};

#endif // MINIMAX_AI_H
