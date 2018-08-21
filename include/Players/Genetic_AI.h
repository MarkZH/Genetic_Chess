#ifndef GENETIC_AI_H
#define GENETIC_AI_H

#include <iosfwd>
#include <string>

#include "Minimax_AI.h"

#include "Genes/Genome.h"

class Board;
class Move;
class Clock;
struct Game_Tree_Node_Result;
class Game_Result;

class Genetic_AI : public Minimax_AI
{
    public:
        Genetic_AI();
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
        bool operator<(const Genetic_AI& other) const;
        bool operator==(const Genetic_AI& other) const;

    private:
        Genome genome;

        static int next_id;
        int id_number;

        void read_from(std::istream& is);

        double internal_evaluate(const Board& board,
                                 Color perspective,
                                 size_t depth) const override;

        // Time management
        virtual double time_to_examine(const Board& board, const Clock& clock) const override;
        virtual double speculation_time_factor(const Board& board) const override;
};

#endif // GENETIC_AI_H
