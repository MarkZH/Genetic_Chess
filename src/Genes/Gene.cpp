#include "Genes/Gene.h"

#include <cmath>
#include <iostream>

#include "Game/Color.h"
#include "Utility.h"

Gene::Gene() : scalar(0.0), scalar_non_negative(false), active(true)
{
}

void Gene::reset_properties() const
{
    properties["Scalar"] = scalar;
}

void Gene::load_properties()
{
    scalar = properties["Scalar"];
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
        scalar += Random::random_normal(10.0);
        if(scalar_non_negative)
        {
            scalar = std::abs(scalar);
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
        return scalar*score_board(board, perspective);
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

void Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>&)
{
}

bool Gene::is_active() const
{
    return active;
}

void Gene::make_scalar_non_negative()
{
    scalar_non_negative = true;
}
