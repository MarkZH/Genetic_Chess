#include "Genes/Look_Ahead_Gene.h"

#include <map>
#include <cassert>
#include <cmath>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

void Look_Ahead_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties.erase("Priority - Opening");
    properties.erase("Priority - Endgame");
    properties["Mean Game Length"] = mean_game_length;
    properties["Game Length Uncertainty"] = game_length_uncertainty;
    properties["Speculation"] = speculation_constant;
    properties["Branching Factor"] = branching_factor_estimate;
}

void Look_Ahead_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    mean_game_length = properties.at("Mean Game Length");
    game_length_uncertainty = properties.at("Game Length Uncertainty");
    speculation_constant = properties.at("Speculation");
    branching_factor_estimate = properties.at("Branching Factor");
    recalculate_game_lengths();
}

Clock::seconds Look_Ahead_Gene::time_to_examine(const Board& board, const Clock& clock) const noexcept
{
    auto time_left = clock.running_time_left();
    auto moves_to_reset = clock.moves_until_reset(board.whose_turn());
    auto moves_left = expected_moves_left(board);
    return time_left/std::min(moves_left, double(moves_to_reset));
}

void Look_Ahead_Gene::gene_specific_mutation() noexcept
{
    switch(Random::random_integer(1, 4))
    {
        case 1:
            mean_game_length += Random::random_laplace(1.0);
            break;
        case 2:
            game_length_uncertainty += Random::random_laplace(0.01);
            break;
        case 3:
            speculation_constant += Random::random_laplace(0.05);
            break;
        case 4:
            branching_factor_estimate += Random::random_laplace(1.0);
            break;
        default:
            assert(false);
    }

    recalculate_game_lengths();
}

std::string Look_Ahead_Gene::name() const noexcept
{
    return "Look Ahead Gene";
}

double Look_Ahead_Gene::score_board(const Board&, Piece_Color, size_t, double) const noexcept
{
    return 0.0;
}

double Look_Ahead_Gene::speculation_time_factor() const noexcept
{
    return speculation_constant;
}

double Look_Ahead_Gene::branching_factor() const noexcept
{
    return branching_factor_estimate;
}

double Look_Ahead_Gene::expected_moves_left(const Board& board) const noexcept
{
    auto moves_so_far = board.ply_count()/2;
    if(moves_so_far < moves_left_lookup.size())
    {
        return moves_left_lookup[moves_so_far];
    }
    else
    {
        return Math::average_moves_left(mean_game_length, game_length_uncertainty, moves_so_far);
    }
}

void Look_Ahead_Gene::recalculate_game_lengths() noexcept
{
    for(size_t i = 0; i < moves_left_lookup.size(); ++i)
    {
        moves_left_lookup[i] = Math::average_moves_left(mean_game_length, game_length_uncertainty, i);
    }
}
