#include "Genes/Look_Ahead_Gene.h"

#include <cassert>
#include <memory>
#include <algorithm>
#include <stdexcept>

#include "Genes/Gene.h"
#include "Utility.h"
#include "Game/Board.h"
#include "Game/Clock.h"

Look_Ahead_Gene::Look_Ahead_Gene() :
    mean_game_length(50),
    game_length_uncertainty(0.5),
    speculation_constant(0.0),
    can_capture_speculation_constant(0.0),
    did_capture_speculation_constant(0.0)
{
}

void Look_Ahead_Gene::reset_properties() const
{
    properties["Mean Game Length"] = mean_game_length;
    properties["Game Length Uncertainty"] = game_length_uncertainty;
    properties["Speculation Constant"] = speculation_constant;
    properties["Captured Speculation Constant"] = did_capture_speculation_constant;
    properties["Can Capture Speculation Constant"] = can_capture_speculation_constant;
}

void Look_Ahead_Gene::load_properties()
{
    mean_game_length = properties["Mean Game Length"];
    game_length_uncertainty = properties["Game Length Uncertainty"];
    speculation_constant = properties["Speculation Constant"];
    did_capture_speculation_constant = properties["Captured Speculation Constant"];
    can_capture_speculation_constant = properties["Can Capture Speculation Constant"];
}

double Look_Ahead_Gene::time_to_examine(const Board& board, const Clock& clock) const
{
    auto time_left = clock.time_left(board.whose_turn());
    auto moves_to_reset = clock.moves_to_reset(board.whose_turn());

    auto moves_so_far = board.game_record().size()/2; // only count moves by this player
    auto moves_left = Math::average_moves_left(mean_game_length, game_length_uncertainty, moves_so_far);

    return time_left/std::min(moves_left, double(moves_to_reset));
}

void Look_Ahead_Gene::gene_specific_mutation()
{
    switch(Random::random_integer(1, 5))
    {
        case 1:
            mean_game_length = std::max(1.0, mean_game_length + Random::random_laplace(1.0));
            break;
        case 2:
            game_length_uncertainty = std::max(0.0, game_length_uncertainty + Random::random_laplace(0.05));
            break;
        case 3:
            speculation_constant += Random::random_laplace(0.1);
            break;
        case 4:
            did_capture_speculation_constant += Random::random_laplace(0.1);
            break;
        case 5:
            can_capture_speculation_constant += Random::random_laplace(0.1);
            break;
        default:
            throw std::runtime_error("Bad random value in Look Ahead Gene");
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

double Look_Ahead_Gene::score_board(const Board&, const Board&, size_t) const
{
    return 0.0;
}

double Look_Ahead_Gene::speculation_time_factor(const Board& board) const
{
    if(board.capture_possible())
    {
        if(board.last_move_captured())
        {
            return std::max(did_capture_speculation_constant,
                            can_capture_speculation_constant);
        }
        else
        {
            return can_capture_speculation_constant;
        }
    }
    else if(board.last_move_captured())
    {
        return did_capture_speculation_constant;
    }

    return speculation_constant;
}
