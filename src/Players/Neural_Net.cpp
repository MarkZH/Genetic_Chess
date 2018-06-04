#include "Players/Neural_Net.h"

#include <vector>
#include <cassert>
#include <algorithm>

#include "Game/Board.h"
#include "Pieces/Piece.h"

#include "Utility.h"

bool dimensions_are_correct(const Connections& c, size_t rows, size_t columns)
{
    return c.size() == columns && std::all_of(c.begin(), c.end(), [rows](auto v) { return v.size() == rows; });
}

Neural_Net::Neural_Net() :
    hidden_connections(1, {input_node_count, std::vector<double>(input_node_count, 0.0)}),
    output_connections(input_node_count, std::vector<double>(output_node_count, 0.0))
{
    assert(hidden_connections.size() == 1);
    assert(std::all_of(hidden_connections.begin(),
                       hidden_connections.end(),
                       [](auto x)
                       {
                           return dimensions_are_correct(x, input_node_count, input_node_count);
                       }));

    assert(dimensions_are_correct(output_connections, output_node_count, input_node_count));
}

std::vector<double>& operator+=(std::vector<double>& v1, const std::vector<double>& v2)
{
    assert(v1.size() == v2.size());
    for(size_t i = 0; i < v1.size(); ++i)
    {
        v1[i] += v2[i];
    }
    return v1;
}

Layer get_next_layer(const Connections& connections, const Layer& layer)
{
    std::vector<double> result(connections.front().size(), 0.0);
    for(size_t i = 0; i < layer.size(); ++i)
    {
        if(layer[i] == 1)
        {
            result += connections[i];
        }
    }

    Layer layer_result(result.size());
    std::transform(result.begin(), result.end(), layer_result.begin(),
                   [](auto x) { return x > 1.0 ? 1 : 0; });

    return layer_result;
}

double Neural_Net::evaluate(const Board & board, Color perspective) const
{
    auto layer = get_initial_layer(board);
    for(const auto& c : hidden_connections)
    {
        layer = get_next_layer(c, layer);
    }

    auto final_layer = get_next_layer(output_connections, layer);
    uint64_t bits = 0;
    for(size_t i = 0; i < final_layer.size(); ++i)
    {
        if(final_layer[i] == 1)
        {
            bits += (uint64_t(1) << i);
        }
    }

    return (board.whose_turn() == perspective ? 1.0 : -1.0)*bits;
}

void mutate_2D_array(Connections& arr)
{
    size_t total_mutating_nodes = arr.size()*arr.front().size()/100; // muatate 1% of connections
    for(size_t i = 0; i < total_mutating_nodes; ++i)
    {
        auto index1 = Random::random_integer(0, arr.size() - 1);
        auto index2 = Random::random_integer(0, arr.front().size() - 1);
        arr[index1][index2] += Random::random_normal(0.2);
    }
}

void Neural_Net::mutate(int iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        for(auto& layer : hidden_connections)
        {
            mutate_2D_array(layer);
        }
        mutate_2D_array(output_connections);
    }
}

void print_2D_array(const Connections& arr, std::ostream& output)
{
    for(auto node_iter = arr.begin();
        node_iter != arr.end();
        ++node_iter)
    {
        if(node_iter != arr.begin())
        {
            output << '/';
        }

        for(auto value_iter = node_iter->begin();
            value_iter != node_iter->end();
            ++value_iter)
        {
            if(value_iter != node_iter->begin())
            {
                output << ',';
            }
            output << *value_iter;
        }
    }
}

void Neural_Net::print(std::ostream & output) const
{
    for(const auto& layer : hidden_connections)
    {
        output << "Hidden: ";
        print_2D_array(layer, output);
        output << '\n';
    }

    output << "Output: ";
    print_2D_array(output_connections, output);
    output << "\n\n";
}

Layer Neural_Net::get_initial_layer(const Board & board) const
{
    auto result = Layer(Neural_Net::input_node_count, 0);
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece)
            {
                auto starting_index = piece->color()*(input_node_count/2) + piece->type()*64;
                result[starting_index + board.board_index(file, rank)] = 1;
            }
        }
    }

    return result;
}

Connections read_single_connection(const std::string& text)
{
    Connections result;
    for(auto row : String::split(text, "/"))
    {
        result.push_back({});
        for(auto entry : String::split(row, ","))
        {
            result.back().push_back(std::stod(entry));
        }
    }

    return result;
}

void Neural_Net::read_from(std::istream& is)
{
    hidden_connections.clear();
    output_connections.clear();

    std::string line;
    while(std::getline(is, line))
    {
        if(line.empty())
        {
            continue;
        }

        if(line == "END")
        {
            return;
        }

        auto parse_line = String::split(line, ":");
        if(parse_line.size() != 2)
        {
            throw std::runtime_error("Bad Neural_AI line: " + line);
        }

        auto header = parse_line.front();
        auto data = parse_line.back();
        if(header == "Hidden")
        {
            hidden_connections.push_back(read_single_connection(data));
        }
        else if(header == "Output")
        {
            output_connections = read_single_connection(data);
        }
    }
}
