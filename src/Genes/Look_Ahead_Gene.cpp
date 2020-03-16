#include "Genes/Look_Ahead_Gene.h"

#include <map>
#include <cassert>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

Look_Ahead_Gene::Look_Ahead_Gene() noexcept
{
    zero_out_priority();
}

void Look_Ahead_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties.erase("Priority");
    properties["Mean Game Length"] = mean_game_length;
    properties["Game Length Uncertainty"] = game_length_uncertainty;
    properties["Speculation"] = speculation_constant;
}

void Look_Ahead_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    mean_game_length = properties.at("Mean Game Length");
    game_length_uncertainty = properties.at("Game Length Uncertainty");
    speculation_constant = properties.at("Speculation");
}

double Look_Ahead_Gene::time_to_examine(const Board& board, const Clock& clock) const noexcept
{
    if( ! is_active())
    {
        return clock.running_time_left();
    }

    auto time_left = clock.time_left(board.whose_turn());
    auto moves_to_reset = clock.moves_until_reset(board.whose_turn());

    auto moves_so_far = board.ply_count()/2; // only count moves by this player
    auto moves_left = Math::average_moves_left(mean_game_length, game_length_uncertainty, moves_so_far);

    return time_left/std::min(moves_left, double(moves_to_reset));
}

void Look_Ahead_Gene::gene_specific_mutation() noexcept
{
    switch(Random::random_integer(1, 3))
    {
        case 1:
            mean_game_length += Random::random_laplace(1.0);
            break;
        case 2:
            game_length_uncertainty += Random::random_laplace(0.01);
            break;
        case 3:
            speculation_constant += Random::random_laplace(0.03);
            break;
        default:
            assert(false);
    }
}

std::string Look_Ahead_Gene::name() const noexcept
{
    return "Look Ahead Gene";
}

double Look_Ahead_Gene::score_board(const Board&, Piece_Color, size_t) const noexcept
{
    return 0.0;
}

double Look_Ahead_Gene::speculation_time_factor() const noexcept
{
    if(is_active())
    {
        return speculation_constant;
    }
    else
    {
        return 0.0;
    }
}
