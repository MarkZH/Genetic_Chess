#ifndef GENETIC_AI_H
#define GENETIC_AI_H

#include <vector>
#include <iosfwd>
#include <set>

#include "Player.h"
#include "Genes/Genome.h"

class Board;
class Clock;

enum Color;

class Genetic_AI : public Player
{
    public:
        Genetic_AI();
        explicit Genetic_AI(const std::string& file_name); // read genome from file
        Genetic_AI(const std::string& file_name, int id); // read genome from gene pool file with ID
        explicit Genetic_AI(std::istream& is); // read genome from file
        explicit Genetic_AI(const Genetic_AI& gai_mother,
                            const Genetic_AI& gai_father); // offspring with random recombination of genes
        virtual ~Genetic_AI() override;

        void mutate();
        const Complete_Move choose_move(const Board& board, const Clock& clock) const override;

        std::string name() const override;
        void print_genome(const std::string& file_name = "") const;
        void print_genome(std::ostream& file) const;

        int get_id() const;

    private:
        Genome genome;
        static int next_id;
        int id;

        void read_from(std::istream& is);

        Board get_final_board_state(const Board& board,
                                    const Complete_Move& next_move,
                                    double positions_to_examine,
                                    const Clock& clock) const;
};

#endif // GENETIC_AI_H
