#include "Genes/Look_Ahead_Gene.h"

#include <cmath>

#include "Utility.h"
#include "Game/Board.h"
#include "Game/Clock.h"


Look_Ahead_Gene::Look_Ahead_Gene() :
    mean_game_length(50),
    positions_per_second(0)
{
}

void Look_Ahead_Gene::reset_properties() const
{
    properties["Mean Game Length"] = mean_game_length;
    properties["Positions Per Second"] = positions_per_second;
}

void Look_Ahead_Gene::load_properties()
{
    mean_game_length = properties["Mean Game Length"];
    positions_per_second = properties["Positions Per Second"];
}

Look_Ahead_Gene::~Look_Ahead_Gene()
{
}

int Look_Ahead_Gene::positions_to_examine(const Board& board, const Clock& clock) const
{
    auto time_left = clock.time_left(board.whose_turn());
    auto moves_to_reset = clock.moves_to_reset(board.whose_turn());

    auto moves_so_far = board.get_game_record().size()/2; // only count moves by this player
    auto moves_left = Math::average_moves_left(mean_game_length, moves_so_far);

    auto time_per_move = time_left/std::min(moves_left, double(moves_to_reset));
    return int(positions_per_second*time_per_move); // positions to examine for one move
}

void Look_Ahead_Gene::mutate()
{
    if(Random::coin_flip())
    {
        mean_game_length = std::max(1.0, mean_game_length + Random::random_normal(1.0));
    }
    else
    {
        positions_per_second = std::max(0.0, positions_per_second + Random::random_normal(10.0));
    }
}

Look_Ahead_Gene* Look_Ahead_Gene::duplicate() const
{
    return new Look_Ahead_Gene(*this);
}

std::string Look_Ahead_Gene::name() const
{
    return "Look Ahead Gene";
}

double Look_Ahead_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}
