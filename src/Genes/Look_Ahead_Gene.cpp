#include "Genes/Look_Ahead_Gene.h"

#include <cmath>
#include <cassert>
#include <memory>

#include "Genes/Gene.h"
#include "Utility.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Moves/Move.h"


Look_Ahead_Gene::Look_Ahead_Gene() :
    mean_game_length(50),
    game_length_uncertainty(0.5),
    speculation_constant(0.5),
    capturing_speculation_constant(0.5)
{
    recalculate_exponent();
}

void Look_Ahead_Gene::reset_properties() const
{
    properties["Mean Game Length"] = mean_game_length;
    properties["Game Length Uncertainty"] = game_length_uncertainty;
    properties["Speculation Constant"] = speculation_constant;
    properties["Capturing Speculation Constant"] = capturing_speculation_constant;
}

void Look_Ahead_Gene::load_properties()
{
    mean_game_length = properties["Mean Game Length"];
    game_length_uncertainty = properties["Game Length Uncertainty"];
    speculation_constant = properties["Speculation Constant"];
    capturing_speculation_constant = properties["Capturing Speculation Constant"];
    recalculate_exponent();
}

double Look_Ahead_Gene::time_to_examine(const Board& board, const Clock& clock) const
{
    auto time_left = clock.time_left(board.whose_turn());
    auto moves_to_reset = clock.moves_to_reset(board.whose_turn());

    auto moves_so_far = board.get_game_record().size()/2; // only count moves by this player
    auto moves_left = Math::average_moves_left(mean_game_length, game_length_uncertainty, moves_so_far);

    return time_left/std::min(moves_left, double(moves_to_reset));
}

void Look_Ahead_Gene::gene_specific_mutation()
{
    switch(Random::random_integer(1, 4))
    {
        case 1:
            mean_game_length = std::max(1.0, mean_game_length + Random::random_normal(1.0));
            break;
        case 2:
            game_length_uncertainty = std::abs(game_length_uncertainty + Random::random_normal(0.05));
            break;
        case 3:
            speculation_constant += Random::random_normal(0.05);
            speculation_constant = Math::clamp(speculation_constant, 0.0, 1.0);
            recalculate_exponent();
            break;
        case 4:
            capturing_speculation_constant += Random::random_normal(0.05);
            capturing_speculation_constant = Math::clamp(capturing_speculation_constant, 0.0, 1.0);
            recalculate_exponent();
            break;
        default:
            assert(false); // If here, random_integer() called with wrong parameters
            break;
    }
}

std::unique_ptr<Gene> Look_Ahead_Gene::duplicate() const
{
    return std::make_unique<Look_Ahead_Gene>(*this);
}

std::string Look_Ahead_Gene::name() const
{
    return "Look Ahead Gene";
}

double Look_Ahead_Gene::score_board(const Board&) const
{
    return 0.0;
}

bool Look_Ahead_Gene::enough_time_to_recurse(double time_allotted, const Board& board, double positions_per_second) const
{
    // The idea is that, if the time allotted to a move is less than the time
    // this gene thinks it takes to examine every move of the resulting board state
    // without recursion, then it should still recurse with a probability that is a
    // function of the ratio of the time allotted to time needed. The speculation_constant
    // specifies how often this should happen, with 0 being never and 1 being always
    // (see the recalculate_exponent() function for the math).

    auto minimum_time_to_recurse = board.legal_moves().size()/positions_per_second;
    auto base = time_allotted/minimum_time_to_recurse;

    if(base <= 0.0)
    {
        return false;
    }

    if(base >= 1.0)
    {
        return true;
    }

    auto exponent = (board.capture_possible() ? capturing_speculation_exponent : speculation_exponent);
    return Random::success_probability(std::pow(base, exponent));
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

    if(capturing_speculation_constant > 0.0)
    {
        capturing_speculation_exponent = (1.0 - capturing_speculation_constant)/capturing_speculation_constant;
    }
    else
    {
        capturing_speculation_exponent = Math::infinity;
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
