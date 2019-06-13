#ifndef GENETIC_AI_H
#define GENETIC_AI_H

#include <iosfwd>
#include <string>
#include <map>

#include "Minimax_AI.h"

#include "Genes/Genome.h"
#include "Game/Color.h"

class Board;
class Clock;

//! This classes uses evolutionary algorithms to learn how to play chess.
//
//! This is the player for which this program is named.
class Genetic_AI : public Minimax_AI
{
    public:
        //! Generate a randomly mutated Genetic_AI
        //
        //! \param mutation_count The number of genome mutations to apply to the AI after construction.
        explicit Genetic_AI(int mutation_count);

        //! Create a Genetic_AI from a text file by searching for a specfic ID.
        //
        //! \param file_name The name of the file with the Genetic_AI data.
        //! \param id_in The ID to search for.
        //! \throws Genetic_AI_Creation_Error If there is an error during reading.
        Genetic_AI(const std::string& file_name, int id);
        
        //! Create a new Genetic_AI via sexual reproduction.
        //
        //! The offspring is formed by randomly taking genes from each parent.
        //! \param A The first parent.
        //! \param B The second parent.
        Genetic_AI(const Genetic_AI& gai_mother,
                   const Genetic_AI& gai_father);

        //! Apply random mutations to the Genome of the Genetic_AI
        //
        //! \param mutation_count The number of mutation actions to apply to the genome. Defaults to 1 if unspecified.
        void mutate(int mutation_count = 1);

        //! Reports the name of the AI and ID number.
        //
        //! \returns "Genetic AI" plus the ID.
        std::string name() const override;
        
        //! Reports the author of this chess engine.
        //
        //! \returns "Mark Harrison"
        std::string author() const override;

        //! Prints the information defining this AI.
        //
        //! The printed information includes the ID number, ancestry information, and genetic data.
        //! \param file_name The name of the text file to print to. If empty, print to stdout.
        void print(const std::string& file_name = "") const;
        
        //! Print AI information to the given std::ostream.
        //
        //! \param os The stream to be written to.
        void print(std::ostream& file) const;

        //! Reports the ID number of the Genetic_AI.
        //
        //! \returns The ID number.
        int id() const;

        //! This function identifies the first gene pool into which a newly created Genetic_AI is placed.
        //
        //! \param pool_id The ID number of the gene pool.
        void set_origin_pool(size_t pool_id);

        //! A comparison function to sort Genetic_AI collections.
        //
        //! \param other Another Genetic_AI.
        //! \returns If the other AI should go after this AI.
        bool operator<(const Genetic_AI& other) const;

    private:
        Genome genome;

        static int next_id;
        static size_t max_origin_pool_id;

        int id_number;
        std::map<size_t, double> ancestry; // record mix of parents' original gene pool ancestry

        void read_data(std::istream& is);
        void read_ancestry(std::istream& is);

        double internal_evaluate(const Board& board,
                                 Color perspective,
                                 size_t depth) const override;

        // Time management
        double time_to_examine(const Board& board, const Clock& clock) const override;
        double speculation_time_factor(const Board& board) const override;
};

#endif // GENETIC_AI_H
