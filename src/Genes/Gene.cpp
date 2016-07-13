#include "Genes/Gene.h"

#include <cmath>

#include "Game/Color.h"
#include "Utility.h"

Gene::Gene(double initial_scalar_value) : scalar(initial_scalar_value)
{
}

void Gene::reset_base_properties() const
{
    properties["Scalar"] = scalar;
}

void Gene::load_base_properties()
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
    scalar += Random::random_normal(10.0);
}

double Gene::evaluate(const Board& board, Color perspective) const
{
    return scalar*score_board(board, perspective);
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

void Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>&)
{
}
