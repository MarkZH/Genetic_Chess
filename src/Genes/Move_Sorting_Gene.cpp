#include "Genes/Move_Sorting_Gene.h"

#include <vector>
#include <functional>
#include <string>
#include <algorithm>
#include <set>

#include "Game/Board.h"
#include "Game/Move.h"

#include "Utility/Random.h"
#include "Utility/String.h"
#include "Utility/Exceptions.h"

namespace
{
    const std::string input_list_delimiter = ",";
    const auto output_list_delimiter = input_list_delimiter + " ";
    const auto count_property = "Sorter Count";
    const auto order_property = "Sorter Order";
}

Move_Sorter::Move_Sorter(const std::string& name_in, sorter_t&& sorter_in) noexcept :
    name(name_in),
    sorter(sorter_in)
{
}

Move_Sorting_Gene::Move_Sorting_Gene() noexcept :
    Clonable_Gene("Move Sorting Gene"),
    move_sorters{{
        {"Force Changers", [](const Move& move, const Board& board) { return board.move_changes_material(move); }},
        {"Attack Dodgers", [](const Move& move, const Board& board) { return board.attacked_by(move.start(), opposite(board.whose_turn())); }},
        {"Pawn Pushers",   [](const Move& move, const Board& board) { return board.piece_on_square(move.start()).type() == Piece_Type::PAWN; }},
        {"King Checkers",  [](const Move& move, const Board& board) { return board.move_checks_king(move) || board.is_discovered_check(move); }},
        {"King Castlers",  [](const Move& move, const Board&)       { return move.is_castle(); }}
    }}
{
}

double Move_Sorting_Gene::score_board(const Board&, Piece_Color, size_t) const noexcept
{
    return 0.0;
}

void Move_Sorting_Gene::gene_specific_mutation() noexcept
{
    if(Random::coin_flip())
    {
        if(sorter_count == 0)
        {
            sorter_count += (Random::coin_flip() ? 1 : 0);
        }
        else if(sorter_count == move_sorters.size())
        {
            sorter_count += (Random::coin_flip() ? -1 : 0);
        }
        else
        {
            sorter_count += (Random::coin_flip() ? -1 : 1);
        }
    }
    else
    {
        const auto swap_location = Random::random_integer<size_t>(0, move_sorters.size() - 2);
        std::swap(move_sorters[swap_location], move_sorters[swap_location + 1]);
    }
}

void Move_Sorting_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    delete_priorities(properties);
    std::vector<std::string> sorter_list;
    std::transform(move_sorters.begin(), move_sorters.end(), std::back_inserter(sorter_list), [](const auto& ms) { return ms.name; });
    properties[order_property] = String::join(sorter_list, output_list_delimiter);
    properties[count_property] = std::to_string(sorter_count);
}

void Move_Sorting_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    auto sorter_names = String::split(properties.at(order_property), input_list_delimiter);
    std::transform(sorter_names.begin(), sorter_names.end(), sorter_names.begin(), String::remove_extra_whitespace);

    const auto name_set = std::set(sorter_names.begin(), sorter_names.end());
    if(name_set.size() != sorter_names.size())
    {
        throw Genome_Creation_Error("Sorter name list contains duplicates: " + properties.at(order_property));
    }

    auto current_sorter = move_sorters.begin();
    for(const auto& name : sorter_names)
    {
        auto sorter = std::find_if(current_sorter, move_sorters.end(), [&name](const auto& ms) { return ms.name == name; });
        if(sorter == move_sorters.end())
        {
            throw Genome_Creation_Error(name + " is not a move sorter name.");
        }
        std::iter_swap(current_sorter, sorter);
        ++current_sorter;
    }

    sorter_count = String::to_number<size_t>(properties.at(count_property));
    sorter_count = std::min(sorter_count, sorter_names.size());
}
