#include "Genes/Look_Ahead_Gene.h"

#include <map>
#include <memory>
#include <cassert>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

Look_Ahead_Gene::Look_Ahead_Gene() :
    speculation_constants{},
    mean_game_length(50),
    game_length_uncertainty(0.5)
{
    speculation_constants.fill(1.0);
}

std::map<std::string, double> Look_Ahead_Gene::list_properties() const
{
    std::map<std::string, double> properties;
    properties["Mean Game Length"] = mean_game_length;
    properties["Game Length Uncertainty"] = game_length_uncertainty;
    properties["Speculation-Default"] = speculation_constants[0];
    properties["Speculation-Just Changed Material"] = speculation_constants[1];
    properties["Speculation-Can Change Material"] = speculation_constants[2];
    properties["Speculation-Material Exchange"] = speculation_constants[3];
    return properties;
}

void Look_Ahead_Gene::load_properties(const std::map<std::string, double>& properties)
{
    mean_game_length = properties.at("Mean Game Length");
    game_length_uncertainty = properties.at("Game Length Uncertainty");
    speculation_constants[0] = properties.at("Speculation-Default");
    speculation_constants[1] = properties.at("Speculation-Just Changed Material");
    speculation_constants[2] = properties.at("Speculation-Can Change Material");
    speculation_constants[3] = properties.at("Speculation-Material Exchange");
}

double Look_Ahead_Gene::time_to_examine(const Board& board, const Clock& clock) const
{
    auto time_left = clock.time_left(board.whose_turn());
    auto moves_to_reset = clock.moves_until_reset(board.whose_turn());

    auto moves_so_far = board.game_record().size()/2; // only count moves by this player
    auto moves_left = Math::average_moves_left(mean_game_length, game_length_uncertainty, moves_so_far);

    return time_left/std::min(moves_left, double(moves_to_reset));
}

void Look_Ahead_Gene::gene_specific_mutation()
{
    auto choice = size_t(Random::random_integer(1, 6));
    switch(choice)
    {
        case 1:
            mean_game_length += Random::random_laplace(0.3);
            mean_game_length = std::abs(mean_game_length);
            break;
        case 2:
            game_length_uncertainty += Random::random_laplace(0.01);
            game_length_uncertainty = std::abs(game_length_uncertainty);
            break;
        default:
            assert(choice >= 3 && choice - 3 < speculation_constants.size());
            auto& spec = speculation_constants[choice - 3];
            spec += Random::random_laplace(0.01);
            spec = std::max(spec, 1.0);
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

double Look_Ahead_Gene::score_board(const Board&, Color, size_t) const
{
    return 0.0;
}

double Look_Ahead_Gene::speculation_time_factor(const Board& board) const
{
    size_t index = 0;

    // Each extra board factor adds another power of two
    if(board.last_move_changed_material()) { index += 1; }
    if(board.material_change_possible())   { index += 2; }

    return speculation_constants[index];
}
