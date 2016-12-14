#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>

#include "Game/Color.h"
#include "Piece_Strength_Gene.h"

class Board;
class Gene;
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

        double evaluate(const Board& board, Color perspective) const;
        void mutate();

        int positions_to_examine(const Board& board, const Clock& clock) const; // how many moves in future to evaluate board

        void print(std::ostream& os) const;

    private:
        std::vector<std::shared_ptr<Gene>> genome;
        std::map<std::string, bool> gene_active;

        // Regulatory gene locations
        size_t piece_strength_gene_index;
        size_t look_ahead_gene_index;

        double score_board(const Board& board, Color perspective) const;
        void reseat_piece_strength_gene();
};

#endif // GENOME_H
