#include "Genes/Search_Strategy_Gene.h"

#include <string>
#include <algorithm>

#include "Genes/Gene_Value.h"
#include "Utility/Random.h"
#include "Utility/Exceptions.h"

namespace
{
    const auto search_method_names =
        []()
        {
            std::array<std::string, 2> names;
            names[static_cast<int>(Search_Method::MINIMAX)] = "Minimax";
            names[static_cast<int>(Search_Method::ITERATIVE_DEEPENING)] = "Iterative Deepening";
            return names;
        }();
}

std::string Search_Strategy_Gene::name() const noexcept
{
    return "Search Strategy Gene";
}

Search_Method Search_Strategy_Gene::searching_method() const noexcept
{
    return search_method.value();
}

double Search_Strategy_Gene::score_board(const Board&, Piece_Color, size_t, double) const noexcept
{
    return 0.0;
}

void Search_Strategy_Gene::gene_specific_mutation() noexcept
{
    if(Random::success_probability(1, 100))
    {
        search_method.value() = search_method.value() == Search_Method::MINIMAX ?
                                Search_Method::ITERATIVE_DEEPENING :
                                Search_Method::MINIMAX;
    }
}

void Search_Strategy_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    delete_priorities(properties);
    search_method.write_to_map(properties, search_method_names.at(static_cast<int>(search_method.value())));
}

void Search_Strategy_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    const auto search_method_name_input = properties.at(search_method.name());
    const auto method = std::find(search_method_names.begin(),
                                  search_method_names.end(),
                                  search_method_name_input);

    if(method == search_method_names.end())
    {
        throw Genome_Creation_Error("Unrecognized search method: " + search_method_name_input);
    }

    search_method.value() = static_cast<Search_Method>(std::distance(search_method_names.begin(), method));
}
