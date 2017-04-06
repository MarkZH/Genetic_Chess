#include "Genes/Gene.h"

#include <cmath>
#include <iostream>

#include "Game/Color.h"
#include "Utility.h"

Gene::Gene() : priority(0.0), priority_non_negative(false), active(true)
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
        if(line.empty())
        {
            break;
        }

        if(line.find("ACTIVE") != std::string::npos)
        {
            active = (line == "ACTIVE");
            break;
        }

        auto split_line = String::split(line, ": ");
        auto property_name = split_line[0];
        auto property_value = std::atof(split_line[1].c_str());
        try
        {
            properties.at(property_name) = property_value;
        }
        catch(const std::out_of_range&)
        {
            throw_on_invalid_line(line, "Unrecognized parameter");
        }
    }

    load_properties();
}

void Gene::throw_on_invalid_line(const std::string& line, const std::string& reason) const
{
    throw std::runtime_error("Invalid line in while reading for " + name() + ": " + line + "\n" + reason);
}


Gene::~Gene()
{
}

void Gene::mutate()
{
    if(Random::success_probability(0.95))
    {
        priority += Random::random_normal(10.0);
        if(priority_non_negative)
        {
            priority = std::max(priority, 0.0);
        }
        gene_specific_mutation();
    }
    else
    {
        active = ! active;
    }
}

void Gene::gene_specific_mutation()
{
}

double Gene::evaluate(const Board& board, Color perspective) const
{
    if(is_active())
    {
        return priority*score_board(board, perspective);
    }
    else
    {
        return 0.0;
    }
}

void Gene::print(std::ostream& os) const
{
    reset_properties();
    os << "\nName: " << name() << "\n";
    for(const auto& name_value : properties)
    {
        os << name_value.first << ": " << name_value.second << "\n";
    }
    os << (is_active() ? "" : "IN") << "ACTIVE\n";
}

void Gene::reset_piece_strength_gene(const Piece_Strength_Gene*)
{
}

bool Gene::is_active() const
{
    return active;
}

void Gene::make_priority_non_negative()
{
    priority_non_negative = true;
}
