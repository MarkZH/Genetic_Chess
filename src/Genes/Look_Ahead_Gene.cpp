#include "Genes/Look_Ahead_Gene.h"

#include <map>
#include <memory>
#include <cassert>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Utility.h"

Look_Ahead_Gene::Look_Ahead_Gene() :
    speculation_constants{},
    mean_game_length(50),
    game_length_uncertainty(0.5)
{
}

void Look_Ahead_Gene::reset_properties() const
{
    properties["Mean Game Length"] = mean_game_length;
    properties["Game Length Uncertainty"] = game_length_uncertainty;
    properties["Speculation-Default"] = speculation_constants[0];
    properties["Speculation-Just Captured"] = speculation_constants[1];
    properties["Speculation-Can Capture"] = speculation_constants[2];
    properties["Speculation-Recapture"] = speculation_constants[3];
}

void Look_Ahead_Gene::load_properties()
{
    mean_game_length = properties["Mean Game Length"];
    game_length_uncertainty = properties["Game Length Uncertainty"];
    speculation_constants[0] = properties["Speculation-Default"];
    speculation_constants[1] = properties["Speculation-Just Captured"];
    speculation_constants[2] = properties["Speculation-Can Capture"];
    speculation_constants[3] = properties["Speculation-Recapture"];
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
    auto choice = Random::random_integer(1, 6);
    switch(choice)
    {
        case 1:
            mean_game_length += Random::random_laplace(1.0);
            mean_game_length = std::abs(mean_game_length);
            break;
        case 2:
            game_length_uncertainty += Random::random_laplace(0.05);
            game_length_uncertainty = std::abs(game_length_uncertainty);
            break;
        default:
            assert(choice >= 3 && size_t(choice) < speculation_constants.size() + 3);
            auto& spec = speculation_constants[choice - 3];
            spec += Random::random_laplace(0.1);
            spec = std::abs(spec);
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

double Look_Ahead_Gene::score_board(const Board&, const Board&, size_t) const
{
    return 0.0;
}

double Look_Ahead_Gene::speculation_time_factor(const Board& board) const
{
    size_t index = 0;

    // Each extra board factor adds another power of two
    if(board.last_move_captured()) { index += 1; }
    if(board.capture_possible())   { index += 2; }

    return speculation_constants[index];
}
