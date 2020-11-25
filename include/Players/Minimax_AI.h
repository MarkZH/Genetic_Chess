#ifndef MINIMAX_AI_H
#define MINIMAX_AI_H

#include "Player.h"

#include <vector>
#include <array>
#include <map>
#include <utility>

#include "Game/Color.h"
#include "Game/Clock.h"

#include "Players/Game_Tree_Node_Result.h"
#include "Utility/Fixed_Capacity_Vector.h"

class Board;
class Move;
class Game_Result;

//! \file

//! \brief This Player uses a variable-depth minimax algorithm with alpha-beta pruning.
//!
//! Minimax algorithm: https://en.wikipedia.org/wiki/Minimax
//! Alpha-beta pruning: https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
class Minimax_AI : public Player
{
    public:
        //! \brief Minimax_AI uses a variable-depth minimax algorithm with alpha-beta pruning.
        //!
        //! The depth of the search is determined by how much time is available.
        //! At first, equal time is allocated to each legal move for examination. Time
        //! is overcommitted with the expectation that not all of the time will be used
        //! due to search cutoffs due to alpha-beta pruning.
        //! \param board The current state of the game.
        //! \param clock The game clock telling how much time is left in the game.
        const Move& choose_move(const Board& board, const Clock& clock) const noexcept override;

        //! \brief Prints the expected future variation and score for the chosen move.
        //!
        //! \param board The state of the game just prior to the move being commented on.
        //! \param move_number The numeric label to use for the move (in case the current
        //!        game did not start with move 1.
        std::string commentary_for_next_move(const Board& board, size_t move_number) const noexcept override;

        void undo_move(const Move* last_move) const noexcept override;

        void reset() const noexcept override;

    protected:
        //! \brief Recalculate values that will last the lifetime of the instance.
        //!
        //! In this case, the values are an initial estimate of the speed of
        //! searching the game tree and the value of a centipawn for reporting
        //! scores of board positions.
        void recalibrate_self() const noexcept;

    private:
        //! The working memory during search_game_tree() for predicted variations
        //! indexed by this AI's move choice and then by the opponent's response.
        mutable std::map<const Move*, std::map<const Move*, Game_Tree_Node_Result>> depth_two_results;

        //! A list of predicted variation results indexed by the opponent's move choices.
        //! This is created by picking the set of variations from depth_two_results once
        //! the AI has chosen a move.
        mutable std::map<const Move*, Game_Tree_Node_Result> depth_one_results;

        //! \brief Data for writing commentary for each move choice to PGN files.
        //!
        //! Each entry is a pair of results of the game search tree. The first is the predicted variation
        //! found while the AI searched for its own move. The second, if it is not default constructed,
        //! is the variation that follows the AI's opponent's actual next move.
        mutable std::vector<std::pair<Game_Tree_Node_Result, Game_Tree_Node_Result>> commentary;

        // Monitor search speed to adapt to different computers/competing workloads
        mutable size_t nodes_searched;
        mutable std::chrono::steady_clock::time_point clock_start_time;
        mutable size_t maximum_depth;

        // For thinking output
        mutable int nodes_evaluated;
        mutable Clock::seconds total_evaluation_time;
        mutable Clock::seconds node_evaluation_time;
        mutable std::chrono::steady_clock::time_point time_at_last_output;

        // Evaluation method
        double evaluate(const Board& board,
                        const Game_Result& move_result,
                        Piece_Color perspective,
                        size_t depth) const noexcept;
        virtual double internal_evaluate(const Board& board,
                                         Piece_Color perspective,
                                         size_t depth) const  noexcept = 0;

        virtual const std::array<double, 6>& piece_values() const noexcept = 0;

        // Time management
        virtual Clock::seconds time_to_examine(const Board& board, const Clock& clock) const noexcept = 0;
        virtual double speculation_time_factor(const Board& board) const noexcept = 0;
        virtual double branching_factor() const noexcept = 0;

        // Scoring output
        double centipawn_value() const noexcept;

        // Current sequence of moves as game tree is traversed.
        const static size_t maximum_search_depth = 100; // to prevent stack overflow
        const static size_t maximum_quiescent_captures = 32; // to prevent overflow of current_variation_store
        const static size_t variation_store_size = maximum_search_depth + maximum_quiescent_captures;
        using current_variation_store = Fixed_Capacity_Vector<const Move*, variation_store_size>;

        // Minimax (actually negamax) with alpha-beta pruning
        Game_Tree_Node_Result search_game_tree(const Board& board,
                                               Clock::seconds time_to_examine,
                                               size_t minimum_search_depth,
                                               const Clock& clock,
                                               Game_Tree_Node_Result alpha,
                                               const Game_Tree_Node_Result& beta,
                                               std::vector<const Move*>& principal_variation,
                                               current_variation_store& current_variation) const noexcept;

        //! \brief Assign a score to the current board state.
        //!
        //! \param board The current state of the Board.
        //! \param extra_moves A list of moves to submit to the Board before scoring.
        //!        (e.g., moves that result in a quiescent Board).
        //! \param perspective From whose perspective (Black or White) the board should be scored.
        //! \param move_result The possibly game-ending result of the move.
        //! \param move_list The current move list from the game tree search.
        Game_Tree_Node_Result create_result(const Board& board,
                                            Piece_Color perspective,
                                            const Game_Result& move_result,
                                            const current_variation_store& move_list) const noexcept;

        // Output thinking to stdout
        void output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                  Piece_Color perspective) const noexcept;
        void output_thinking_uci(const Game_Tree_Node_Result& thought,
                                 Piece_Color perspective) const noexcept;

        std::chrono::duration<double> time_since_last_output() const noexcept;

        mutable double value_of_centipawn;

        //! \brief Approximate the value of 0.01 pawns for reporting scores.
        //!
        //! \returns A numerical value to normalize the scores returned by board evaluations
        //!          so that the loss of a random pawn changes the score by about 1.0.
        void calculate_centipawn_value() const noexcept;

        //! \brief Initial measurement of evaluation speed of the engine.
        //!
        //! The method Minimax_AI::choose_move() keeps track of the time it takes
        //! and the number of positions it evaluates. But, it needs an accurate
        //! starting value for the first move search. So, this practice move will
        //! update the evaluation speed to a more reasonable starting value.
        void calibrate_thinking_speed() const noexcept;
};

//! \brief Create a PGN variation string.
//!
//! \param board The board state just before any of the variation moves are played.
//! \param move_number The index of the game move (0 for the first player's first move,
//!        1 for first move by that player's opponent, etc.).
//! \param variation A sequence of moves from the current board position.
//! \param score The score assigned to the resulting board after the sequence of moves.
//! \param alternate_variation Another sequence of moves starting from the same board position.
//! \param alternate_score The score assigned to the board after the alternate sequence of moves.
std::string variation_line(Board board,
                           size_t move_number,
                           const std::vector<const Move*>& variation,
                           double score,
                           const std::vector<const Move*>& alternate_variation,
                           double alternate_score);

#endif // MINIMAX_AI_H
