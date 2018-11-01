#include "Genes/Gene.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "Game/Board.h"
#include "Game/Color.h"
#include "Utility.h"

Gene::Gene() : scoring_priority(0.0), exponent(1.0)
{
}

void Gene::reset_properties() const
{
    properties["Priority"] = scoring_priority;
    properties["Exponent"] = exponent;
}

void Gene::load_properties()
{
    scoring_priority = properties["Priority"];
    exponent = properties["Exponent"];
}

size_t Gene::mutatable_components() const
{
    reset_properties();
    return properties.size();
}

void Gene::read_from(std::istream& is)
{
    reset_properties();

    std::string line;
    while(std::getline(is, line))
    {
        line = String::strip_comments(line, '#');
        if(line.empty())
        {
            break;
        }

        auto split_line = String::split(line, ":");
        if(split_line.size() != 2)
        {
            throw_on_invalid_line(line, "There should be exactly one colon per gene property line.");
        }
        auto property_name = String::trim_outer_whitespace(split_line[0]);
        auto property_data = String::trim_outer_whitespace(split_line[1]);
        if(property_name == "Name")
        {
            if(property_data == name())
            {
                continue;
            }
            else
            {
                throw std::runtime_error("Reading data for wrong gene. Gene is " + name() + ", data is for " + property_data + ".");
            }
        }

        try
        {
            properties.at(property_name) = std::stod(property_data);
        }
        catch(const std::out_of_range&)
        {
            throw_on_invalid_line(line, "Unrecognized parameter.");
        }
        catch(const std::invalid_argument&)
        {
            throw_on_invalid_line(line, "Bad parameter value.");
        }
    }

    load_properties();
}

void Gene::read_from(const std::string& file_name)
{
    auto ifs = std::ifstream(file_name);
    std::string line;

    while(std::getline(ifs, line))
    {
        if(String::starts_with(line, "Name: "))
        {
            auto gene_name = String::trim_outer_whitespace(String::split(line, ":", 1)[1]);
            if(gene_name == name())
            {
                read_from(ifs);
                return;
            }
        }
    }

    throw std::runtime_error(name() + " not found in " + file_name);
}


void Gene::throw_on_invalid_line(const std::string& line, const std::string& reason) const
{
    throw std::runtime_error("Invalid line in while reading for " + name() + ": " + line + "\n" + reason);
}

void Gene::mutate()
{
    scoring_priority += Random::random_laplace(10.0);
    exponent += Random::random_laplace(0.01);
    gene_specific_mutation();
}

void Gene::gene_specific_mutation()
{
}

double Gene::evaluate(const Board& board, const Board& opposite_board, size_t depth) const
{
    auto score = score_board(board, opposite_board, depth);
    return Math::sign(score)*scoring_priority*std::pow(std::abs(score), exponent);
}

void Gene::print(std::ostream& os) const
{
    reset_properties();
    os << "Name: " << name() << "\n";
    for(const auto& name_value : properties)
    {
        os << name_value.first << ": " << name_value.second << "\n";
    }
    os << '\n';
}

void Gene::reset_piece_strength_gene(const Piece_Strength_Gene*)
{
}

bool Gene::test(const Board& board, double expected_score) const
{
    auto other_board = board;
    other_board.set_turn(opposite(board.whose_turn()));
    auto result = score_board(board, other_board, 1);
    if(std::abs(result - expected_score) > 1e-6)
    {
        std::cerr << "Error in " << name() << ": Expected " << expected_score << ", Got: " << result << '\n';
        return false;
    }

    return true;
}

double Gene::priority() const
{
    return scoring_priority;
}

void Gene::make_priority_minimum_zero()
{
    scoring_priority = std::max(scoring_priority, 0.0);
}
