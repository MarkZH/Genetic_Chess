#ifndef NEURAL_AI_H
#define NEURAL_AI_H

#include "Minimax_AI.h"

#include <string>
#include <iosfwd>

#include "Neural_Net.h"

class Neural_AI : public Minimax_AI
{
    public:
        Neural_AI();

        std::string name() const override;
        std::string author() const override;

        size_t get_id() const;

        void mutate(int iterations = 1);

        void print(const std::string& file_name) const;
        void print(std::ostream& output) const;

    private:
        Neural_Net brain;
        static size_t next_id;
        size_t id;

        double internal_evaluate(const Board& board, Color perspective) const override;

        double speculation_time_factor(const Board& board) const override;
        double time_to_examine(const Board& board, const Clock& clock) const override;
};

#endif // NEURAL_AI_H
