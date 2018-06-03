#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include <vector>
#include <iosfwd>

#include "Game/Color.h"

class Board;

using Connections = std::vector<std::vector<double>>;
using Layer = std::vector<int>;

class Neural_Net
{
    public:
        Neural_Net();

        double evaluate(const Board& board, Color perspective) const;
        void mutate(int iterations = 1);

        void print(std::ostream& output) const;

    private:
        const static size_t input_node_count = 64*6*2; // (board squares)*(piece types)*(colors)
        const static size_t output_node_count = 64;
        
        std::vector<Connections> hidden_connections;
        Connections output_connections;

        Layer get_initial_layer(const Board& board) const;
};

#endif // NEURAL_NET_H
