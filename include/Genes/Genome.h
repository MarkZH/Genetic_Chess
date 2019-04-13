#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>

#include "Game/Color.h"
#include "Gene.h"

class Board;
class Move;
class Clock;

//! A software analog to a biological chromosome containing a collection of Gene instances that control the Genetic_AI behavior.
class Genome
{
    public:
        Genome(); // random settings on all genes
        Genome(const Genome& other); // clone
        Genome(const Genome& mother, const Genome& father); // randomly combine from parents
        Genome& operator=(const Genome& other);

        void read_from(std::istream& is);

        double evaluate(const Board& board, Color perspective, size_t depth) const;
        void mutate();

        double time_to_examine(const Board& board, const Clock& clock) const; // how much time to use for this move
        double speculation_time_factor(const Board& board) const;

        void print(std::ostream& os) const;

    private:
        std::vector<std::unique_ptr<Gene>> genome;

        // Regulatory gene locations
        static size_t piece_strength_gene_index;
        static size_t look_ahead_gene_index;
        static size_t mutation_rate_gene_index;

        double score_board(const Board& board, Color perspective, size_t depth) const;
        void reset_piece_strength_gene();
        double components_to_mutate() const;
};

#endif // GENOME_H
