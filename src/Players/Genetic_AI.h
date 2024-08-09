#ifndef MINIMAX_AI_H
#define MINIMAX_AI_H

#include "Player.h"

#include <vector>
#include <array>
#include <map>
#include <utility>
#include <string>
#include <iosfwd>

#include "Game/Color.h"
#include "Game/Clock.h"

#include "Players/Game_Tree_Node_Result.h"
#include "Players/Alpha_Beta_Value.h"
#include "Players/Move_Decision.h"
#include "Utility/Fixed_Capacity_Vector.h"
#include "Genes/Genome.h"

class Board;
class Move;
class Game_Result;

//! \file

//! \brief This Player uses a variable-depth minimax algorithm with alpha-beta pruning. Scoring and time controlled are evolved via a genetic algorithm.
//!
//! Minimax algorithm: https://en.wikipedia.org/wiki/Minimax
//! Alpha-beta pruning: https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
class Genetic_AI : public Player
{
    public:
        //! Default construct a Genetic_AI
        Genetic_AI() noexcept = default;

        //! Load an AI from a file.
        //!
        //! \param file_name The name of the file containing the AI.
        //! \param id The AI ID to search for in the file.
        Genetic_AI(const std::string& file_name, int id);

        //! Load an AI from an opened stream.
        //!
        //! \param is The already opened input stream.
        //! \param id The AI ID to search for in the stream.
        Genetic_AI(std::istream& is, int id);

        //! Load an AI from an opened stream.
        //!
        //! \param is The already opened input stream.
        //! \param id The AI ID to search for in the stream.
        Genetic_AI(std::istream&& is, int id);

        //! Create a new AI by mating two existing ones.
        //!
        //! \param a The first AI.
        //! \param b The second AI.
        Genetic_AI(const Genetic_AI& a, const Genetic_AI& b) noexcept;

        std::string name() const noexcept override;
        std::string author() const noexcept override;

        //! A numeric identifier for this AI.
        int id() const noexcept;

        //! \brief Genetic_AI uses a variable-depth minimax algorithm with alpha-beta pruning.
        //!
        //! The depth of the search is determined by how much time is available.
        //! At first, equal time is allocated to each legal move for examination. Time
        //! is overcommitted with the expectation that not all of the time will be used
        //! due to search cutoffs due to alpha-beta pruning.
        //! \param board The current state of the game.
        //! \param clock The game clock telling how much time is left in the game.
        Move_Decision choose_move(const Board& board, const Clock& clock) const noexcept override;

        //! \brief Prints the expected future variation and score for the chosen move.
        //!
        //! \param board The state of the game just prior to the move being commented on.
        //! \param move_number The numeric label to use for the move (in case the current
        //!        game did not start with move 1.
        std::string commentary_for_next_move(const Board& board, size_t move_number) const noexcept override;

        void undo_move(const Move* last_move) const noexcept override;

        void reset() const noexcept override;

        //! \brief Randomly mutate the AI.
        //!
        //! \param mutation_rate The number of discrete mutations to apply to the AI
        void mutate(size_t mutation_rate) noexcept;

        //! \brief Print the AI parameters to a file.
        //!
        //! \param file_name The name of a file.
        void print(const std::string& file_name) const;

        //! \brief Print the AI parameters to an output stream
        //!
        //! \param os The output stream
        void print(std::ostream& os) const noexcept;

        //! \brief Ordering operator for std::map
        //!
        //! \param other The AI being compared to this one.
        bool operator<(const Genetic_AI& other) const noexcept;

    private:
        //! \brief The maximum depth to search to limit the size of the current_variation_store.
        //!
        //! Also prevents stack overflow.
        const static size_t maximum_variation_depth = 100;

        //! \brief The maximum search depth for finding quiescent positions.
        const static size_t maximum_quiescent_captures = 32;

        //! \brief The total size of the current_variation_store.
        const static size_t variation_store_size = maximum_variation_depth + maximum_quiescent_captures;

        //! A datatype for storing the moves that are played to reach the current board position during a search.
        using current_variation_store = Fixed_Capacity_Vector<const Move*, variation_store_size>;

        //! \brief Recalculate values that will last the lifetime of the instance.
        //!
        //! In this case, the values are an initial estimate of the speed of
        //! searching the game tree and the value of a centipawn for reporting
        //! scores of board positions.
        void recalibrate_self() const noexcept;

        //! \brief Resets the values of internal search stats (node counts, time used, etc.) before the next search.
        //!
        //! \param board The current board position. If this is a new game, delete previous commentary.
        void reset_search_stats(const Board& board) const noexcept;

        //! \brief Output final stats for the move chosen and record the commentary for the chosen move.
        //!
        //! \param result The data (variation and score) for the chosen move.
        //! \param board The current board position.
        void report_final_search_stats(const Game_Tree_Node_Result& result, const Board& board) const noexcept;

        //! \brief Returns how much time to spend choosing this move.
        //!
        //! \param board The curren board position.
        //! \param clock The game clock.
        //! \returns A time duration indicating how much time to use.
        Clock::seconds time_to_examine(const Board& board, const Clock& clock) const noexcept;

        //! \brief An estimate of the average number of moves that will be searched per board position.
        //!
        //! \param game_progress An estimate of how much of the game has been played (0.0 at the beginning, 1.0 at the end).
        double branching_factor(double game_progress) const noexcept;

        //! \brief An amount by which to overestimate the time to use.
        //!
        //! \param game_progress An estimate of how much of the game has been played (0.0 at the beginning, 1.0 at the end).
        double speculation_time_factor(double game_progress) const noexcept;

        //! \brief An estimate of how much of the game has been played (0.0 at the beginning, 1.0 at the end).
        //!
        //! \param board The current board position.
        double game_progress(const Board& board) const noexcept;

        //! \brief Search the game tree using the minimax (actually negamax) algorithm with alpha-beta pruning
        //!
        //! \param board The current board position.
        //! \param time_to_examine How much time to use choosing this move.
        //! \param minimum_search_depth The minimum depth to search before evaluating a variation.
        //! \param maximum_search_depth The maximum depth to search.
        //! \param clock The game clock.
        //! \param alpha The current value for alpha: the best variation score found that the current player can force.
        //! \param beta The current value for beta: the variation score that, if the current variation scores better, will result
        //!        in the opponent choosing different earlier moves to avoid the current variation.
        //! \param principal_variation The best line found from the previous search--used to order moves in the current search.
        //! \param current_variation The list of moves to reach the current board position.
        //! \returns The best variation and its score.
        Game_Tree_Node_Result search_game_tree(const Board& board,
                                               Clock::seconds time_to_examine,
                                               size_t minimum_search_depth,
                                               size_t maximum_search_depth,
                                               const Clock& clock,
                                               double progress_of_game,
                                               Alpha_Beta_Value alpha,
                                               const Alpha_Beta_Value& beta,
                                               std::vector<const Move*>& principal_variation,
                                               current_variation_store& current_variation) const noexcept;

        // The brains of the Minimax algorithm that provides board evaluation and time management.
        Genome genome;

        // Data for writing commentary for each move choice to PGN files.
        mutable std::vector<Game_Tree_Node_Result> commentary;

        // Monitor search speed to adapt to different computers/competing workloads
        mutable Clock::seconds node_evaluation_time;
        mutable size_t nodes_searched;
        mutable std::chrono::steady_clock::time_point clock_start_time;
        mutable size_t maximum_depth;

        // For thinking output
        mutable int nodes_evaluated;
        mutable Clock::seconds total_evaluation_time;
        mutable std::chrono::steady_clock::time_point time_at_last_output;

        //! \brief Sort moves before searching further in the game tree.
        //! \tparam Iter An iterator type that points to a const Move*.
        //! \param begin An iterator to the beginning of the move list to be sorted.
        //! \param end An iterator to the end of the move list to be sorted.
        //! \param board The board from which the move list is derived.
        template<typename Iter>
        void sort_moves(Iter begin, Iter end, const Board& board, const double game_progress) const noexcept
        {
            genome.sort_moves(begin, end, board, game_progress);
        }

        // Evaluation method
        double assign_score(const Board& board,
                            const Game_Result& move_result,
                            Piece_Color perspective,
                            size_t depth) const noexcept;
        double internal_evaluate(const Board& board,
                                 Piece_Color perspective,
                                 size_t depth) const noexcept;

        const std::array<double, 6>& piece_values() const noexcept;

        // Scoring output
        double centipawn_value() const noexcept;

        Game_Tree_Node_Result evaluate(const Game_Result& move_result,
                                       Board& next_board,
                                       Genetic_AI::current_variation_store& current_variation,
                                       Piece_Color perspective,
                                       std::chrono::steady_clock::time_point evaluate_start_time) const noexcept;

        bool search_further(const Game_Result& move_result,
                            size_t depth,
                            const Board& next_board,
                            const std::vector<const Move*>& principal_variation,
                            size_t minimum_search_depth,
                            size_t maximum_search_depth,
                            Clock::seconds time_allotted_for_this_move) const noexcept;

        //! \brief Assign a score to the current board state.
        //!
        //! \param board The current state of the Board.
        //! \param perspective From whose perspective (Black or White) the board should be scored.
        //! \param move_result The possibly game-ending result of the move.
        //! \param move_list The current move list from the game tree search.
        Game_Tree_Node_Result create_result(const Board& board,
                                            Piece_Color perspective,
                                            const Game_Result& move_result,
                                            const current_variation_store& move_list) const noexcept;

        // Output thinking to stdout
        void output_thinking(const Game_Tree_Node_Result& thought,
                             Piece_Color perspective) const noexcept;
        void output_thinking_xboard(const Game_Tree_Node_Result& thought,
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
        //! The method Genetic_AI::choose_move() keeps track of the time it takes
        //! and the number of positions it evaluates. But, it needs an accurate
        //! starting value for the first move search. So, this practice move will
        //! update the evaluation speed to a more reasonable starting value.
        void calibrate_thinking_speed() const noexcept;

        Move_Decision choose_move_minimax(const Board& board, const Clock& clock) const noexcept;

        std::vector<const Move*> get_legal_principal_variation(const Board& board) const noexcept;
};

//! \brief Find the last ID of a Genome in a gene pool file.
//!
//! \param file_name The name of the file with Genome data.
//! \returns The numerical ID of the last AI in the file.
//! \exception std::runtime_error if no valid ID line can be found.
int find_last_id(const std::string& file_name);

#endif // MINIMAX_AI_H
