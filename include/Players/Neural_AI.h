#ifndef NEURAL_AI_H
#define NEURAL_AI_H

#include "Minimax_AI.h"

#include <string>
#include <iosfwd>

#include "Neural_Net.h"
#include "Genes/Look_Ahead_Gene.h"

class Neural_AI : public Minimax_AI
{
    public:
        Neural_AI();
        explicit Neural_AI(const std::string& file_name);
        Neural_AI(const std::string& file_name, int id);
        explicit Neural_AI(std::istream& is);

        Neural_AI copy() const; // make copy with new ID

        std::string name() const override;
        std::string author() const override;

        size_t get_id() const;

        void mutate(int iterations = 1);

        void print(const std::string& file_name) const;
        void print(std::ostream& output) const;

        bool operator<(const Neural_AI& other) const;
        bool operator==(const Neural_AI& other) const;

    private:
        static size_t next_id;
        size_t id;

        Neural_Net brain;
        Look_Ahead_Gene time_manager;

        void read_from(std::istream& input);
        void read_internals(std::istream& input);

        double internal_evaluate(const Board& board, Color perspective) const override;

        double speculation_time_factor(const Board& board) const override;
        double time_to_examine(const Board& board, const Clock& clock) const override;
};

#endif // NEURAL_AI_H
