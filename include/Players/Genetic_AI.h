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

//! This is the player for which this program is named.
class Genetic_AI : public Minimax_AI
{
    public:
        explicit Genetic_AI(int mutation_count);
        explicit Genetic_AI(const std::string& file_name); // read genome from file
        Genetic_AI(const std::string& file_name, int id); // read genome from gene pool file with ID
        explicit Genetic_AI(std::istream& is); // read genome from file
        Genetic_AI(const Genetic_AI& gai_mother,
                   const Genetic_AI& gai_father); // offspring with random recombination of genes

        void mutate(int mutation_count = 1);

        std::string name() const override;
        std::string author() const override;

        void print(const std::string& file_name = "") const;
        void print(std::ostream& file) const;

        int id() const;
        void set_origin_pool(int pool_id);

        bool operator<(const Genetic_AI& other) const;

    private:
        Genome genome;

        static int next_id;
        static int max_origin_pool_id;

        int id_number;
        std::map<int, double> ancestry; // record mix of parents' original gene pool ancestry

        void read_from(std::istream& is);
        void read_ancestry(std::istream& is);

        double internal_evaluate(const Board& board,
                                 Color perspective,
                                 size_t depth) const override;

        // Time management
        double time_to_examine(const Board& board, const Clock& clock) const override;
        double speculation_time_factor(const Board& board) const override;
};

#endif // GENETIC_AI_H
