#include "Genes/Look_Ahead_Gene.h"

#include <cmath>

#include "Genes/Gene.h"
#include "Utility.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Moves/Complete_Move.h"


Look_Ahead_Gene::Look_Ahead_Gene() :
    mean_game_length(50),
    positions_per_second(0.01),
    speculation_constant(0.0),
    minimum_moves_left(0)
{
    recalculate_exponent();
}

void Look_Ahead_Gene::reset_properties() const
{
    properties["Mean Game Length"] = mean_game_length;
    properties["Positions Per Second"] = positions_per_second;
    properties["Speculation Constant"] = speculation_constant;
    properties["Minimum Moves Left"] = minimum_moves_left;
}

void Look_Ahead_Gene::load_properties()
{
    mean_game_length = properties["Mean Game Length"];
    positions_per_second = properties["Positions Per Second"];
    speculation_constant = properties["Speculation Constant"];
    minimum_moves_left = properties["Minimum Moves Left"];
    recalculate_exponent();
}

double Look_Ahead_Gene::time_to_examine(const Board& board, const Clock& clock) const
{
    if( ! is_active())
    {
        return 0.0;
    }

    auto time_left = clock.time_left(board.whose_turn());
    auto moves_to_reset = clock.moves_to_reset(board.whose_turn());

    auto moves_so_far = board.get_game_record().size()/2; // only count moves by this player
    auto moves_left = Math::average_moves_left(mean_game_length, moves_so_far);
    moves_left = std::max(moves_left, minimum_moves_left);

    return time_left/std::min(moves_left, double(moves_to_reset));
}

void Look_Ahead_Gene::gene_specific_mutation()
{
    auto choice = Random::random_integer(1, 4);
    switch(choice)
    {
        case 1:
            mean_game_length = std::max(1.0, mean_game_length + Random::random_normal(1.0));
            break;
        case 2:
            positions_per_second = std::max(0.01, positions_per_second + Random::random_normal(10.0));
            break;
        case 3:
            speculation_constant += Random::random_normal(0.01);
            speculation_constant = std::max(speculation_constant, 0.0);
            speculation_constant = std::min(speculation_constant, 1.0);
            recalculate_exponent();
            break;
        case 4:
            minimum_moves_left = std::max(0.0, minimum_moves_left + Random::random_normal(1.0));
            break;
        default:
            throw std::runtime_error("Bad Look_Ahead_Gene mutation: " + std::to_string(choice));
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

double Look_Ahead_Gene::minimum_time_to_recurse(const Board& board) const
{
    if(is_active())
    {
        return board.legal_moves().size()/positions_per_second;
    }
    else
    {
        return Math::infinity;
    }
}

bool Look_Ahead_Gene::enough_time_to_recurse(double time_allotted, const Board& board) const
{
    // The idea is that, if the time allotted to a move is less than the time
    // this gene thinks it takes to examine every move of the resulting board state
    // without recursion, then it should still recurse with a probability that is a
    // function of the ratio of the time allotted to time needed. The speculation_constant
    // specifies how often this should happen, with 0 being never and 1 being always
    // (see the recalculate_exponent() function for the math).

    auto base = time_allotted/minimum_time_to_recurse(board);

    if(base <= 0.0)
    {
        return false;
    }

    if(base >= 1.0)
    {
        return true;
    }

    return Random::success_probability(std::pow(base, speculation_exponent));
}

void Look_Ahead_Gene::recalculate_exponent()
{
    if(speculation_constant > 0.0)
    {
        speculation_exponent = (1.0 - speculation_constant)/speculation_constant;
    }
    else
    {
        speculation_exponent = Math::infinity;
    }

    // constant = 0.0 ==> exponent = infinity
    // constant = 0.5 ==> exponent = 1
    // constant = 1.0 ==> exponent = 0
    //
    // Additionally, the function is symmetric about constant = 0.5:
    //    constant --> 1 - constant ==> exponent --> 1/exponent
    //
    // This results in a value of the std::pow() expression in enough_time_to_recurse()
    //
    //    value = (0 < base < 1)^exponent
    //
    // Large exponents result in values near zero, which means recursion with little time
    // has little probability. Small exponents result in values near 1, so recursion with
    // little time is more likely.
}
