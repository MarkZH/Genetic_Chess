#ifndef MINIMAX_AI_H
#define MINIMAX_AI_H

#include "Player.h"

#include <vector>

#include "Game/Color.h"
#include "Players/Game_Tree_Node_Result.h"

class Board;
class Clock;
class Move;
class Game_Result;

//! This Player uses a variable-depth minimax algorithm with alpha-beta pruning.

//! Minimax algorithm: https://en.wikipedia.org/wiki/Minimax
//! Alpha-beta pruning: https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
class Minimax_AI : public Player
{
    public:
        const Move& choose_move(const Board& board, const Clock& clock) const override;
        std::string commentary_for_next_move(const Board& board) const override;

    protected:
        void calculate_centipawn_value();
        void calibrate_thinking_speed() const;

    private:
        mutable std::vector<const Move*> principal_variation;
        mutable std::vector<Game_Tree_Node_Result> commentary;

        // Monitor search speed to adapt to different computers/competing workloads
        mutable int nodes_searched;
        mutable double clock_start_time;
        mutable size_t maximum_depth;

        // For thinking output
        mutable int nodes_evaluated;
        mutable double total_evaluation_time;
        mutable double evaluation_speed;
        double value_of_centipawn;
        mutable double time_at_last_output;

        // Evaluation method
        double evaluate(const Board& board,
                        const Game_Result& move_result,
                        Color perspective,
                        size_t depth) const;
        virtual double internal_evaluate(const Board& board,
                                         Color perspective,
                                         size_t depth) const = 0;

        // Time management
        virtual double time_to_examine(const Board& board, const Clock& clock) const = 0;
        virtual double speculation_time_factor(const Board& board) const = 0;

        // Scoring output
        double centipawn_value() const;

        // Minimax (actually negamax) with alpha-beta pruning
        Game_Tree_Node_Result search_game_tree(const Board& board,
                                               double time_to_examine,
                                               const Clock& clock,
                                               size_t depth,
                                               Game_Tree_Node_Result alpha,
                                               const Game_Tree_Node_Result& beta,
                                               bool still_on_principal_variation) const;

        Game_Tree_Node_Result create_result(const Board& board,
                                            Color perspective,
                                            const Game_Result& move_result,
                                            size_t depth) const;

        // Output thinking to stdout
        void output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                  const Clock& clock,
                                  Color perspective) const;
        double time_since_last_output(const Clock& clock) const;
};

#endif // MINIMAX_AI_H
