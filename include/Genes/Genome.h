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

        size_t positions_to_examine(double time) const; // how many moves in future to evaluate board
        double time_required() const; // minimum time to continue analysis
        double minimum_score_change() const; // if the change in the a board's score is less than
                                             // this, stop looking ahead and abandon the move sequence

        void print(std::ostream& os) const;

    private:
        std::vector<std::shared_ptr<Gene>> genome;
        std::map<std::string, bool> gene_active;

        // Regulatory gene locations
        size_t piece_strength_gene_index;
        size_t look_ahead_gene_index;
        size_t last_minute_panic_gene_index;
        size_t branch_pruning_gene_index;

        double score_board(const Board& board, Color perspective) const;
        void reseat_piece_strength_gene();
};

#endif // GENOME_H
