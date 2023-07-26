#ifndef GENOME_H
#define GENOME_H

#include <array>
#include <memory>
#include <iosfwd>
#include <cassert>

#include "Game/Color.h"
#include "Game/Clock.h"

#include "Gene.h"
#include "Genes/Move_Sorting_Gene.h"

class Board;
class Move;

//! \brief A software analog to a biological chromosome containing a collection of Gene instances that control the chess player's behavior.
class Genome
{
    public:
        //! \brief Create a genome with default-constructed (neutral behavior) Genes.
        Genome() noexcept;

        //! \brief Clone a genome
        //!
        //! \param other The originating genome.
        Genome(const Genome& other) noexcept;

        //! \brief Move-constructor
        Genome(Genome&& other) = default;

        //! \brief Construct a genome from a file
        //!
        //! \param is The input stream from the opened file.
        //! \param id_in The id number of the Genome to search for.
        Genome(std::istream& is, int id_in);

        //! \brief Create a new genome from two existing genomes via mating
        //!
        //! Create each Gene by copying from either parent with a 50-50 probability.
        //! \param A The first parent.
        //! \param B The second parent.
        Genome(const Genome& A, const Genome& B) noexcept;

        //! \brief Inject another genome's data into this one (i.e., assignment operator)
        //!
        //! \param other The originating genome.
        Genome& operator=(const Genome& other) noexcept;

        //! \brief Move-assignement.
        Genome& operator=(Genome&& other) = default;

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
        double evaluate(const Board& board, Piece_Color perspective, size_t depth) const noexcept;

        //! \brief Apply a number of random mutation to one gene in the genome.
        //!
        //! \param mutation_count The number of times to mutate the genome.
        void mutate(size_t mutation_count) noexcept;

        //! \brief The numeric ID of the owner of this genome
        int id() const noexcept;

        //! \brief Reports the name of the AI and ID number.
        //!
        //! \returns "Genetic Chess" plus the ID.
        std::string name() const noexcept;

        //! \brief Reports the author of this chess engine.
        //!
        //! \returns "Mark Harrison"
        std::string author() const noexcept;

        //! \brief Determine how much time should be used to choose a move.
        //!
        //! \param board The current board position.
        //! \param clock The game clock.
        Clock::seconds time_to_examine(const Board& board, const Clock& clock) const noexcept; // how much time to use for this move

        //! \brief Returns a factor that is multiplied by the time allocated to examine a branch of the game tree.
        //!
        //! \param game_progress An estimate of the amount of the game that has been played on a scale from 0.0 to 1.0.
        //! With alpha-beta pruning, the search for the best move often cuts off before the entire branch
        //! of the game tree is explored, using less time than expected. The value returned by this function
        //! is multiplied by the time allocated for a branch to make sure the time allocated is actually used,
        //! allowing deeper searches of the game tree. More or less time may be allocated based on the
        //! board position being examined.
        //! \returns The multiplicative factor.
        double speculation_time_factor(double game_progress) const noexcept;

        //! \brief Returns an estimate of the number of moves in an average board position (i.e., the branching factor of the game tree).
        //!
        //! \param game_progress An estimate of the amount of the game that has been played on a scale from 0.0 to 1.0.
        double branching_factor(double game_progress) const noexcept;

        //! \brief Estimates how much of the game has happened on the given board.
        //!
        //! \param board The current state of the game.
        double game_progress(const Board& board) const noexcept;

        //! \brief The value of pieces as determined by the Piece_Strength_Gene
        const std::array<double, 6>& piece_values() const noexcept;

        //! \brief Print the genome data to the output stream (std::ofstream, std::cout, etc.).
        //!
        //! \param os The output stream.
        void print(std::ostream& os) const noexcept;

    private:
        int id_number;
        std::array<std::unique_ptr<Gene>, 14> genome;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept;
        void reset_piece_strength_gene() noexcept;
        double expected_number_of_moves_left(const Board& board) const noexcept;

        template<typename Gene_Type>
        constexpr const Gene_Type& gene_reference() const noexcept
        {
            assert(Gene_Type::genome_index < genome.size());
            return static_cast<const Gene_Type&>(*genome[Gene_Type::genome_index]);
        }

    public:
        //! \brief Sort moves before searching further in the game tree.
        //! \tparam Iter An iterator type that points to a const Move*.
        //! \param begin An iterator to the beginning of the move list to be sorted.
        //! \param end An iterator to the end of the move list to be sorted.
        //! \param board The board from which the move list is derived.
        template<typename Iter>
        void sort_moves(Iter begin, Iter end, const Board& board) const noexcept
        {
            gene_reference<Move_Sorting_Gene>().sort_moves(begin, end, board);
        }
};

#endif // GENOME_H
