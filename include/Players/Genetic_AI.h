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

        //! \brief Create a Genetic_AI from a text file by searching for a specfic ID.
        //!
        //! \param file_name The name of the file with the Genetic_AI data.
        //! \param id The ID to search for.
        //! \exception Genetic_AI_Creation_Error or derivative if there is an error during reading.
        Genetic_AI(const std::string& file_name, int id);

        //! \brief Create a Genetic_AI from an already open input stream (as from std::ifstream(file_name)).
        //!
        //! \param is The input stream that is the source of genetic data.
        //! \param id The id of the Genetic_AI to be created.
        //! \exception Genetic_AI_Creation_Error or derivative if the stream cannot be read of if the stream does not contain
        //!         the relevant AI data.
        Genetic_AI(std::istream& is, int id);

        //! \brief Create a Genetic_AI from a (possibly temporary) input stream (as from std::ifstream(file_name)).
        //!
        //! \param is The input stream that is the source of genetic data.
        //! \param id The id of the Genetic_AI to be created.
        //! \exception Genetic_AI_Creation_Error or derivative if the stream cannot be read of if the stream does not contain
        //!         the relevant AI data.
        Genetic_AI(std::istream&& is, int id);

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

        //! \brief Prints the information defining this AI.
        //!
        //! The printed information includes the ID number and genetic data.
        //! \param file_name The name of the text file to print to. If empty, print to stdout.
        void print(const std::string& file_name = {}) const noexcept;

        //! \brief Print AI information to the given std::ostream.
        //!
        //! \param output The stream to be written to.
        void print(std::ostream& output) const noexcept;

        //! \brief Reports the ID number of the Genetic_AI.
        //!
        //! \returns The ID number.
        int id() const noexcept;

        //! \brief A comparison function to sort Genetic_AI collections by ID number.
        //!
        //! \param other Another Genetic_AI.
        //! \returns If the other AI should go after this AI.
        bool operator<(const Genetic_AI& other) const noexcept;

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

    private:
        Genome genome;

        static int next_id;

        int id_number;

        void read_from(std::istream& is);
        void read_data(std::istream& is);
};

//! \brief Find the last ID of a Genetic_AI in a gene pool file.
//!
//! \param file_name The name of the file with Genetic_AI data.
//! \returns The numerical ID of the last AI in the file.
//! \exception std::runtime_error if no valid ID line can be found.
int find_last_id(const std::string& file_name);

#endif // GENETIC_AI_H
