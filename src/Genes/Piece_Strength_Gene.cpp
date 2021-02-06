#include "Genes/Piece_Strength_Gene.h"

#include <string>
using namespace std::string_literals;
#include <array>
#include <map>
#include <algorithm>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

class Board;

Piece_Strength_Gene::Piece_Strength_Gene() noexcept
{
    opening_piece_strength.fill(100.0);
    endgame_piece_strength.fill(100.0);
    renormalize_values();
}

void Piece_Strength_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties.erase("Priority - Opening");
    properties.erase("Priority - Endgame");

    const auto standard_all_pieces_score = 8*1.0 + // pawns
                                           2*5.0 + // rooks
                                           2*3.0 + // knights
                                           2*3.0 + // bishops
                                           1*9.0;  // queen

    auto record_piece_strengths = [this, standard_all_pieces_score, &properties](const auto& piece_strength, const std::string& suffix)
    {
        auto standardize = (normalization(piece_strength) - std::abs(piece_value(piece_strength, Piece_Type::KING)))/standard_all_pieces_score;

        for(size_t piece_index = 0; piece_index < piece_strength.size(); ++piece_index)
        {
            auto piece = Piece{Piece_Color::WHITE, static_cast<Piece_Type>(piece_index)};
            properties[piece.fen_symbol() + " - "s + suffix] = piece_value(piece_strength, piece.type())/standardize;
        }
    };

    record_piece_strengths(opening_piece_strength, "Opening");
    record_piece_strengths(endgame_piece_strength, "Endgame");
}

void Piece_Strength_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    for(std::string name : {"Opening", "Endgame"})
    {
        auto& piece_strength = name == "Opening" ? opening_piece_strength : endgame_piece_strength;
        for(size_t piece_index = 0; piece_index < piece_strength.size(); ++piece_index)
        {
            auto piece = Piece{Piece_Color::WHITE, static_cast<Piece_Type>(piece_index)};
            piece_value(piece_strength, piece.type()) = properties.at(piece.fen_symbol() + " - "s + name);
        }
    }

    renormalize_values();
}

void Piece_Strength_Gene::gene_specific_mutation() noexcept
{
    Random::random_element(Random::coin_flip() ? opening_piece_strength : endgame_piece_strength) += Random::random_laplace(0.005);
    renormalize_values();
}

void Piece_Strength_Gene::renormalize_values() noexcept
{
    for(auto i = 0; i < 2; ++i)
    {
        auto& piece_strength = i == 0 ? opening_piece_strength : endgame_piece_strength;
        auto normalizing_value = normalization(piece_strength);

        if(normalizing_value > 0.0)
        {
            for(auto& v : piece_strength)
            {
                v /= normalizing_value;
            }
        }
    }
}

double Piece_Strength_Gene::normalization(const std::array<double, 6>& piece_strengths) noexcept
{
    return 8*std::abs(piece_value(piece_strengths, Piece_Type::PAWN)) +
           2*std::abs(piece_value(piece_strengths, Piece_Type::ROOK)) +
           2*std::abs(piece_value(piece_strengths, Piece_Type::KNIGHT)) +
           2*std::abs(piece_value(piece_strengths, Piece_Type::BISHOP)) +
           1*std::abs(piece_value(piece_strengths, Piece_Type::QUEEN)) +
           1*std::abs(piece_value(piece_strengths, Piece_Type::KING));
}

double Piece_Strength_Gene::piece_value(const std::array<double, 6>& piece_strength, Piece_Type type) noexcept
{
    return piece_strength[static_cast<int>(type)];
}

double& Piece_Strength_Gene::piece_value(std::array<double, 6>& piece_strength, Piece_Type type) noexcept
{
    return piece_strength[static_cast<int>(type)];
}

double Piece_Strength_Gene::piece_value(Piece piece, double game_progress) const noexcept
{
    if(piece)
    {
        return Math::interpolate(piece_value(opening_piece_strength, piece.type()),
                                 piece_value(endgame_piece_strength, piece.type()),
                                 game_progress);
    }
    else
    {
        return 0.0;
    }
}

std::array<double, 6> Piece_Strength_Gene::piece_values(double game_progress) const noexcept
{
    auto result = decltype(opening_piece_strength){};
    std::transform(opening_piece_strength.begin(), opening_piece_strength.end(),
                   endgame_piece_strength.begin(),
                   result.begin(),
                   [game_progress](auto str1, auto str2)
                   {
                       return Math::interpolate(str1, str2, game_progress);
                   });
    return result;
}

std::string Piece_Strength_Gene::name() const noexcept
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, Piece_Color, size_t, double) const noexcept
{
    return 0.0;
}
