#include "Genes/Interpolated_Gene_Value.h"

#include <string>
#include <array>
#include <map>
#include <cmath>
#include <format>
#include <utility>

#include "Genes/Gene_Value.h"

#include "Utility/Random.h"

Interpolated_Gene_Value::Interpolated_Gene_Value(const std::string& name,
                                                 const double game_start_value,
                                                 const double game_end_value,
                                                 const double mutation_size) noexcept :
    values{{{std::format("{} - Opening", name), game_start_value, mutation_size},
            {std::format("{} - Endgame", name), game_end_value, mutation_size}}}
{
}

double Interpolated_Gene_Value::interpolate(double game_progress) const noexcept
{
    return std::lerp(opening_value(), endgame_value(), game_progress);
}

double Interpolated_Gene_Value::opening_value() const noexcept
{
    return value_at(Game_Stage::OPENING);
}

double& Interpolated_Gene_Value::opening_value() noexcept
{
    return value_at(Game_Stage::OPENING);
}

double Interpolated_Gene_Value::endgame_value() const noexcept
{
    return value_at(Game_Stage::ENDGAME);
}

double& Interpolated_Gene_Value::endgame_value() noexcept
{
    return value_at(Game_Stage::ENDGAME);
}

double Interpolated_Gene_Value::value_at(const Game_Stage stage) const noexcept
{
    return values[std::to_underlying(stage)].value();
}

double& Interpolated_Gene_Value::value_at(const Game_Stage stage) noexcept
{
    return values[std::to_underlying(stage)].value();
}

std::string Interpolated_Gene_Value::name(Game_Stage stage) const noexcept
{
    return values[std::to_underlying(stage)].name();
}

void Interpolated_Gene_Value::load_from_map(const std::map<std::string, std::string>& properties)
{
    for(auto& val : values) { val.load_from_map(properties); }
}

void Interpolated_Gene_Value::write_to_map(std::map<std::string, std::string>& properties) const noexcept
{
    for(const auto& val : values) { val.write_to_map(properties); }
}

void Interpolated_Gene_Value::mutate() noexcept
{
    values[Random::coin_flip()].mutate();
}
