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
        Genetic_AI(const Genetic_AI& other, bool is_clone = false);
        explicit Genetic_AI(const std::string& file_name); // read genome from file
        Genetic_AI(const std::string& file_name, int id); // read genome from gene pool file with ID
        explicit Genetic_AI(std::istream& is); // read genome from file
        explicit Genetic_AI(const Genetic_AI& gai_mother,
                            const Genetic_AI& gai_father); // offspring with random recombination of genes
		Genetic_AI& operator=(Genetic_AI other);
        virtual ~Genetic_AI() override;

        void mutate();
        const Complete_Move choose_move(const Board& board, const Clock& clock) const override;

        std::string name() const override;
        void print_genome(const std::string& file_name = "") const;
        void print_genome(std::ostream& file) const;

        int get_id() const;

        bool operator==(const Genetic_AI& other) const;
        bool operator!=(const Genetic_AI& other) const;

    private:
        Genome genome;
        static int next_id;
        int id;

        void read_from(std::istream& is);

        double evaluate_board(const Board& board,
                              const Clock& clock,
                              Color perspective,
                              int positions_to_examine,
                              double original_board_score) const;

        const Complete_Move choose_move(const Board& board,
                                        const Clock& clock,
                                        int positions_to_examine) const;
};

#endif // GENETIC_AI_H
