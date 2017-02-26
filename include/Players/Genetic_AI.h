#ifndef GENETIC_AI_H
#define GENETIC_AI_H

#include <iosfwd>
#include <string>

#include "Player.h"

#include "Moves/Move.h"
#include "Game/Color.h"

#include "Genes/Genome.h"

class Board;
class Clock;

struct Game_Tree_Node_Result
{
    Complete_Move move;
    double score;
    Color perspective;
    int depth;
    std::string commentary;

    double corrected_score(Color query) const
    {
        if(query == perspective)
        {
            return score;
        }
        else
        {
            return -score;
        }
    }
};

// Is "a" a better result than "b" from "perspective" color
bool better_than(const Game_Tree_Node_Result& a,
                 const Game_Tree_Node_Result& b,
                 Color perspective);

bool operator==(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b);

class Genetic_AI : public Player
{
    public:
        Genetic_AI();
        explicit Genetic_AI(const std::string& file_name); // read genome from file
        Genetic_AI(const std::string& file_name, int id); // read genome from gene pool file with ID
        explicit Genetic_AI(std::istream& is); // read genome from file
        explicit Genetic_AI(const Genetic_AI& gai_mother,
                            const Genetic_AI& gai_father); // offspring with random recombination of genes
        ~Genetic_AI() override;

        void mutate();
        const Complete_Move choose_move(const Board& board, const Clock& clock) const override;

        std::string name() const override;
        std::string author() const override;

        void print_genome(const std::string& file_name = "") const;
        void print_genome(std::ostream& file) const;

        int get_id() const;
        bool operator<(const Genetic_AI& other) const;
        bool operator==(const Genetic_AI& other) const;

    private:
        Genome genome;
        static int next_id;
        int id;

        void read_from(std::istream& is);

        // Minimax (actually negamax) with alpha-beta pruning
        Game_Tree_Node_Result search_game_tree(const Board& board,
                                               double time_to_examine,
                                               const Clock& clock,
                                               int depth,
                                               Game_Tree_Node_Result alpha,
                                               Game_Tree_Node_Result beta) const;
};

#endif // GENETIC_AI_H
