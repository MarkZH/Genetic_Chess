#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include <iosfwd>

#include "Game/Color.h"
#include "Gene.h"

class Board;
class Clock;

//! \brief A software analog to a biological chromosome containing a collection of Gene instances that control the Genetic_AI behavior.
class Genome
{
    public:
        //! \brief Create a genome with default-constructed (neutral behavior) Genes.
        Genome();

        //! \brief Clone a genome
        //!
        //! \param other The originating genome.
        Genome(const Genome& other);

        //! \brief Create a new genome from two existing genomes via mating
        //!
        //! Create each Gene by copying from either parent with a 50-50 probability.
        //! \param A The first parent.
        //! \param B The second parent.
        Genome(const Genome& B, const Genome& A);

        //! \brief Inject another genome's data into this one (i.e., assignment operator)
        //!
        //! \param other The originating genome.
        Genome& operator=(Genome other);

        //! \brief Read genome data from an input stream (std::ifstream, std::cin, etc.).
        //!
        //! \param is The input stream.
        void read_from(std::istream& is);

        //! \brief Evaluate a board position and return a numerical value.
        //!
        //! The higher the value, the greater the probability that the board position
        //! will lead to victory for the player doing the scoring.
        //!
        //! \param board The board position to be evaluated.
        //! \param perspective The player for whom a higher score means a greater chance of victory.
        //! \param depth The current search depth on the game tree.
        //!        (i.e., at the root of the game tree).
        double evaluate(const Board& board, Piece_Color perspective, size_t depth) const;

        //! \brief Apply a random set of mutations to the entire genome.
        //!
        //! The severity of the mutation is controlled by the Mutation_Rate_Gene.
        void mutate();

        //! \brief Determine how much time should be used to choose a move, that is, for the entire search.
        //!
        //! \param board The current board position.
        //! \param clock The game clock.
        double time_to_examine(const Board& board, const Clock& clock) const; // how much time to use for this move

        //! \brief Returns a factor that is multiplied by the time allocated to examine a branch of the game tree.
        //!
        //! With alpha-beta pruning, the search for the best move often cuts off before the entire branch
        //! of the game tree is explored, using less time than expected. The value returned by this function
        //! is multiplied by the time allocated for a branch to make sure the time allocated is actually used,
        //! allowing deeper searches of the game tree. More or less time may be allocated based on the
        //! board position being examined.
        //! \returns The multiplicative factor.
        double speculation_time_factor() const;

        //! \brief The value of pieces as determined by the Piece_Strength_Gene
        const std::array<double, 6>& piece_values() const;

        //! \brief Decide on a minimum search depth based on the depth of alpha.
        //!
        //! When deciding whether to recurse to deeper parts of the game tree,
        //! the player should always recurse when at a depth less than this
        //! fraction of the current alpha result. This should prevent a move
        //! from being picked based on much less analysis than another.
        double minimum_fraction_of_alpha() const noexcept;

        //! \brief Print the genome data to the output stream (std::ofstream, std::cout, etc.).
        //!
        //! \param os The output stream.
        void print(std::ostream& os) const noexcept;

    private:
        std::vector<std::unique_ptr<Gene>> genome;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept;
        void reset_piece_strength_gene() noexcept;

        //! \brief Consults the Mutation_Rate_Gene to determine how many point mutations to apply to the genome.
        //!
        //! \returns A number of mutations to apply.
        double components_to_mutate() const;
};

#endif // GENOME_H
