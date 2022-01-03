#ifndef GENETIC_AI_H
#define GENETIC_AI_H

#include <iosfwd>
#include <string>

#include "Genes/Genome.h"
#include "Game/Color.h"
#include "Game/Clock.h"

class Board;

//! \file

//! \brief This classes uses evolutionary algorithms to learn how to play chess.
//!
//! This is the player for which this program is named.
class Genetic_AI
{
    public:
        //! \brief Generate a Genetic_AI
        explicit Genetic_AI() noexcept;

        //! \brief Create a Genetic_AI from an already open input stream (as from std::ifstream(file_name)).
        //!
        //! \param is The input stream that is the source of genetic data.
        //! \param id The id of the Genetic_AI to be created.
        //! \exception Genetic_AI_Creation_Error or derivative if the stream cannot be read of if the stream does not contain
        //!         the relevant AI data.
        Genetic_AI(std::istream& is, int id);

        //! \brief Create a new Genetic_AI via mating.
        //!
        //! The offspring is formed by randomly taking genes from each parent.
        //! \param A The first parent.
        //! \param B The second parent.
        Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) noexcept;

        //! \brief Apply random mutations to the Genome of the Genetic_AI
        //!
        //! \param mutation_count The number of mutation actions to apply to the genome.
        void mutate(size_t mutation_count) noexcept;

        //! \brief Reports the name of the AI and ID number.
        //!
        //! \returns "Genetic AI" plus the ID.
        std::string name() const noexcept;

        //! \brief Reports the author of this chess engine.
        //!
        //! \returns "Mark Harrison"
        std::string author() const noexcept;

        //! \brief Print AI information to the given std::ostream.
        //!
        //! \param output The stream to be written to.
        void print(std::ostream& output) const noexcept;

        //! \brief Reports the ID number of the Genetic_AI.
        //!
        //! \returns The ID number.
        int id() const noexcept;

        //! \brief Assign a numeric score to the current board position.
        //!
        //! \param board The current board position
        //! \param perspective The side for which a higher score is better
        //! \param depth The current depth of the game tree search
        //!
        //! \returns A numeric score estimating the advantage that the perspective
        //!          player has in the game.
        double internal_evaluate(const Board& board,
                                 Piece_Color perspective,
                                 size_t depth) const noexcept;

        //! \brief A listing of the piece values according to the internal Genetic_AI
        const std::array<double, 6>& piece_values() const noexcept;

        //! \brief The amount of time to spend searching for a move.
        //!
        //! \param board The current board position
        //! \param clock The game clock
        //!
        //! \returns The time in seconds to spend picking the next move
        Clock::seconds time_to_examine(const Board& board, const Clock& clock) const noexcept;

        //! \brief How much to overestimate the time to spend searching.
        //!
        //! \param game_progress An estimate of how much of the game has been played (0.0 to 1.0).
        //!
        //! \returns A factor to multiply the time allocated for a game tree branch to account for
        //!          alpha-beta cutoffs.
        double speculation_time_factor(double game_progress) const noexcept;

        //! \brief An estimate of the effective number of branches per game tree node.
        //!
        //! \param game_progress An estimate of how much of the game has been played (0.0 to 1.0).
        //!
        //! \returns An estimate of how many moves will be examined in each board position.
        double branching_factor(double game_progress) const noexcept;

        //! \brief An estimate of how much of the game has been played(0.0 to 1.0).
        //!
        //! \param board The current board position.
        double game_progress(const Board& board) const noexcept;

        //! \brief Which minimax search method to use.
        Search_Method search_method() const noexcept;

        //! \brief The name of the minimax search method to use.
        std::string search_method_name() const noexcept;

    private:
        Genome genome;
        int id_number;

        void read_from(std::istream& is);
        void read_data(std::istream& is);
};

#endif // GENETIC_AI_H
