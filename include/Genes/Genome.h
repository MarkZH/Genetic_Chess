#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>

#include "Game/Color.h"
#include "Gene.h"

class Board;
class Game_Result;
class Player;
class Clock;

class Genome
{
    public:
        Genome(); // random settings on all genes
        Genome(const Genome& other); // clone
        Genome(const Genome& mother, const Genome& father); // randomly combine from parents
        Genome& operator=(const Genome& other);

        void read_from(std::istream& is);

        double evaluate(const Board& board, const Game_Result& result, Color perspective) const;
        void mutate();

        double time_to_examine(const Board& board, const Clock& clock) const; // how much time to use for this move
        bool enough_time_to_recurse(double time_allotted,
                                    const Board& board,
                                    double positions_per_second) const;

        void print(std::ostream& os) const;

    private:
        std::vector<std::unique_ptr<Gene>> genome;

        // Regulatory gene locations
        size_t piece_strength_gene_index;
        size_t look_ahead_gene_index;

        double score_board(const Board& board) const;
        void reseat_piece_strength_gene();
};

#endif // GENOME_H
