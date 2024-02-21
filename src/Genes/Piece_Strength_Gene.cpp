#include "Genes/Piece_Strength_Gene.h"

#include <string>
#include <array>
#include <map>
#include <numeric>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Color.h"
#include "Game/Board.h"

#include "Utility/Random.h"
#include "Utility/String.h"

Piece_Strength_Gene::Piece_Strength_Gene() noexcept : Clonable_Gene("Piece Strength Gene")
{
    piece_strength.fill(100.0);
    piece_value(Piece_Type::KING) = 0.0;
    renormalize_values();
}

void Piece_Strength_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    delete_priorities(properties);
    delete_activations(properties);

    constexpr auto standard_all_pieces_score = 2*5.0 + // rooks
                                               2*3.0 + // knights
                                               2*3.0 + // bishops
                                               1*9.0;  // queen
    const auto standardize = file_normalization()/standard_all_pieces_score;

    for(size_t piece_index = 0; piece_index < piece_strength.size() - 1; ++piece_index)
    {
        const auto piece = Piece{Piece_Color::WHITE, static_cast<Piece_Type>(piece_index)};
        const auto name = std::string(1, piece.fen_symbol());
        const auto value = piece_strength[piece_index]/standardize;
        properties[name] = std::to_string(value);
    }
}

void Piece_Strength_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    for(const auto& [name, value] : properties)
    {
        if(name == "Enabled")
        {
            continue;
        }

        piece_value(Piece{name[0]}.type()) = String::to_number<double>(value);
    }
    renormalize_values();
}

void Piece_Strength_Gene::gene_specific_mutation() noexcept
{
    const auto index = Random::random_integer<size_t>(0, piece_strength.size() - 2);
    piece_strength[index] += Random::random_laplace(0.005);
    renormalize_values();
}

void Piece_Strength_Gene::renormalize_values() noexcept
{
    const auto normalizing_value = normalization();
    if(normalizing_value > 0.0)
    {
        for(auto& v : piece_strength)
        {
            v /= normalizing_value;
        }
    }
}

double Piece_Strength_Gene::normalization() const noexcept
{
    constexpr auto piece_counts = std::array<int, 6>{8, 2, 2, 2, 1, 0};
    auto norm_strength = piece_strength;
    std::ranges::transform(norm_strength, norm_strength.begin(), [](const auto x) { return std::abs(x); });
    return std::inner_product(piece_counts.begin(), piece_counts.end(), norm_strength.begin(), 0.0);
}

double Piece_Strength_Gene::file_normalization() const noexcept
{
    return normalization() - 8*std::abs(piece_value(Piece_Type::PAWN));
}

double Piece_Strength_Gene::piece_value(const Piece_Type type) const noexcept
{
    return piece_strength[static_cast<int>(type)];
}

double& Piece_Strength_Gene::piece_value(const Piece_Type type) noexcept
{
    return piece_strength[static_cast<int>(type)];
}

double Piece_Strength_Gene::piece_value(const Piece piece) const noexcept
{
    return piece ? piece_value(piece.type()) : 0.0;
}

const std::array<double, 6>& Piece_Strength_Gene::piece_values() const noexcept
{
    return piece_strength;
}

double Piece_Strength_Gene::score_board(const Board&, Piece_Color, size_t) const noexcept
{
    return 0.0;
}

double Piece_Strength_Gene::game_progress(const Board& board) const noexcept
{
    std::array<double, 2> piece_value_left{};
    for(auto square : Square::all_squares())
    {
        if(const auto piece = board.piece_on_square(square))
        {
            piece_value_left[static_cast<size_t>(piece.color())] += std::abs(piece_value(piece));
        }
    }

    const auto piece_score_left = std::min(piece_value_left[0], piece_value_left[1]);
    return 1.0 - piece_score_left;
}
