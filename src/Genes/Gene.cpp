#include "Genes/Gene.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>
#include <numeric>

#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/String.h"

//! Creates a gene with neutral behavior.
Gene::Gene() : scoring_priority(0.0)
{
}

//! Returns a structure relating a gene property to a numerical value.

//! By default, the only property a gene has is "Priority," a multiplicative factor
//! that controls how influential a gene's score is to the overall evaulation of a
//! board position. This method is overridden by derived Gene classes to either augment
//! or replace this data with more specialized properties.
//! \returns A collection of gene properties with their numerical values.
std::map<std::string, double> Gene::list_properties() const
{
    return {{"Priority", scoring_priority}};
}

//! Reads a properties data structure and loads the data into itself.

//! This method is the counterpart to Gene::list_properties() in that it
//! reads the same data structure as the one produced by Gene::list_properties().
//! This means that this method is also overridden by derived Genes that have
//! different properties.
//! \param property_list A data structure with all the data needed for this gene.
//! \throws std::out_of_range When an expected property is not present in the input.
void Gene::load_properties(const std::map<std::string, double>& property_list)
{
    scoring_priority = property_list.at("Priority");
}

//! Tells how many mutatable components are present in a gene.

//! The more components a gene has, the more likely it will be mutated by a call
//! to Genome::mutate().
//! \returns How many individually mutatable components exist in a gene.
size_t Gene::mutatable_components() const
{
    return list_properties().size();
}

//! Reads an input stream for gene data.

//! Every line should be one of the following:
//! - \<property\> = \<value\> with optional comments at the end preceded by '#'.
//! - blank
//! - A commented line starting with a '#'.
//! \param is An input stream (std::ifstream, std::iostream, or similar).
//! \throws std::runtime_error If there is an invalid line or an unexpected property
void Gene::read_from(std::istream& is)
{
    auto properties = list_properties();

    std::string line;
    while(std::getline(is, line))
    {
        line = String::strip_comments(line, "#");
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
                throw_on_invalid_line(line, "Reading data for wrong gene. Gene is " + name() + ", data is for " + property_data + ".");
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

    try
    {
        load_properties(properties);
    }
    catch(const std::out_of_range&)
    {
        auto parameters = std::accumulate(properties.begin(),
                                          properties.end(),
                                          std::string{},
                                          [](const auto& so_far, const auto& next)
                                          {
                                              return so_far + next.first + "\n";
                                          });

        throw std::runtime_error("Bad parameter input for " + name() + "\n" + parameters);
    }
}

//! Read gene data from a text file.

//! The method creates an input stream from the text file and passes it to the
//! other Gene::read_from().
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

//! Applies a random mutation to the priority of a gene as well as any details in derived genes.
void Gene::mutate()
{
    scoring_priority += Random::random_laplace(10.0);
    gene_specific_mutation();
}

//! A method overridden by derived genes to mutate more specific gene components.

//! By default, this method does nothing.
void Gene::gene_specific_mutation()
{
}

//! Gives a numerical score to the board in the arguments.

//! \param board The state of the board to be evaluated--found at the leaves of the game search tree.
//! \param perspective For which player the board is being scored.
//! \param depth The depth of the game search tree at the time of the evaluation.
//! \returns A numerical score indicating the likelihood that the board in the first argument is winning for board.whose_turn().
double Gene::evaluate(const Board& board, Color perspective, size_t depth) const
{
    return scoring_priority*score_board(board, perspective, depth);
}

//! Outputs gene data to a std::ostream in text form.

//! \param os An output stream (std::ofstream, std::cout, etc.)
void Gene::print(std::ostream& os) const
{
    auto properties = list_properties();
    os << "Name: " << name() << "\n";
    for(const auto& name_value : properties)
    {
        os << name_value.first << ": " << name_value.second << "\n";
    }
    os << "\n";
}

//! Gives the gene the new location of the Piece Strength Gene.

//! When a Genome is copied or assigned to, the Genes that reference
//! the Piece_Strength_Gene need to be told the location of the new
//! copy. For all other genes, this does nothing.
//! \param psg A pointer to the correct Piece Strength Gene.
void Gene::reset_piece_strength_gene(const Piece_Strength_Gene*)
{
}

//! Tests the board-scoring method of the Gene.

//! \param board The board upon which the test takes place.
//! \param expected_score The expected score returned by Gene::score_board().
//! \returns Whether the score returned by Gene::score_board() is withing 1e-6 of the expected value.
bool Gene::test(const Board& board, double expected_score) const
{
    auto result = score_board(board, board.whose_turn(), 1);
    if(std::abs(result - expected_score) > 1e-6)
    {
        std::cerr << "Error in " << name() << ": Expected " << expected_score << ", Got: " << result << '\n';
        return false;
    }

    return true;
}

//! If a mutation causes the priority of a gene to become negative, reverse the sign.

//! For some genes, having the priority vary across positive and negative numbers
//! slows down evolution because it has some other multiplicative factor that can
//! also take on negative or positive values. For example, the Total_Force_Gene priority
//! is effectively multliplied by the scores from the Piece_Strength_Gene. Since switching
//! the sign of both leads to the same behavior, making the priority non-negative cuts
//! off the redundant half of the search space.
void Gene::make_priority_minimum_zero()
{
    scoring_priority = std::abs(scoring_priority);
}
