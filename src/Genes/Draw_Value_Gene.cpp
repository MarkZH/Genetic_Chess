#include "Genes/Draw_Value_Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"
#include "Utility/Random.h"

class Board;

std::string Draw_Value_Gene::name() const noexcept
{
    return "Draw Value Gene";
}

double Draw_Value_Gene::draw_value() const noexcept
{
    return value_of_draw_in_centipawns;
}

double Draw_Value_Gene::score_board(const Board&, Piece_Color, size_t, double) const noexcept
{
    return 0.0;
}

void Draw_Value_Gene::gene_specific_mutation() noexcept
{
    value_of_draw_in_centipawns += Random::random_laplace(3.0);
}

void Draw_Value_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties.erase("Priority - Opening");
    properties.erase("Priority - Endgame");
    properties["Draw Value"] = value_of_draw_in_centipawns/100.0;
}

void Draw_Value_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    value_of_draw_in_centipawns = properties.at("Draw Value")*100.;
}
