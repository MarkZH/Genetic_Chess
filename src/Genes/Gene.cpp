#include "Genes/Gene.h"

#include <cmath>
#include <iostream>

#include "Game/Color.h"
#include "Utility.h"

Gene::Gene() : priority(0.0)
{
}

void Gene::reset_properties() const
{
    properties["Priority"] = priority;
}

void Gene::load_properties()
{
    priority = properties["Priority"];
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
        auto property_name = split_line[0];
        auto property_value = std::stod(split_line[1]);
        try
        {
            properties.at(property_name) = property_value;
        }
        catch(const std::out_of_range&)
        {
            throw_on_invalid_line(line, "Unrecognized parameter.");
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
    priority += Random::random_normal(10.0);
    gene_specific_mutation();
}

void Gene::gene_specific_mutation()
{
}

double Gene::evaluate(const Board& board) const
{
    return priority*score_board(board);
}

void Gene::print(std::ostream& os) const
{
    reset_properties();
    os << "\nName: " << name() << "\n";
    for(const auto& name_value : properties)
    {
        os << name_value.first << ": " << name_value.second << "\n";
    }
}

void Gene::reset_piece_strength_gene(const Piece_Strength_Gene*)
{
}

bool Gene::test(const Board& board, double expected_score) const
{
    auto result = score_board(board);
    if(std::abs(result - expected_score) > 1e-6)
    {
        std::cerr << "Error in " << name() << ": Expected " << expected_score << ", Got: " << result << '\n';
        return false;
    }

    return true;
}
