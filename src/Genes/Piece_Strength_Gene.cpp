#include "Genes/Piece_Strength_Gene.h"

#include <string>
#include <array>
#include <map>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Color.h"
#include "Game/Board.h"

#include "Utility/Random.h"
#include "Utility/String.h"

Piece_Strength_Gene::Piece_Strength_Gene() noexcept
{
    piece_strength.fill(100.0);
    renormalize_values();
}

void Piece_Strength_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    delete_priorities(properties);

    static constexpr auto standard_all_pieces_score = 2*5.0 + // rooks
                                                      2*3.0 + // knights
                                                      2*3.0 + // bishops
                                                      1*9.0;  // queen
    const auto standardize = file_normalization()/standard_all_pieces_score;

    for(size_t piece_index = 0; piece_index < piece_strength.size(); ++piece_index)
    {
        const auto piece = Piece{Piece_Color::WHITE, static_cast<Piece_Type>(piece_index)};
        properties[std::string(1, piece.fen_symbol())] = std::to_string(piece_value(piece.type())/standardize);
    }
}

void Piece_Strength_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    for(size_t piece_index = 0; piece_index < piece_strength.size(); ++piece_index)
    {
        const auto piece = Piece{Piece_Color::WHITE, static_cast<Piece_Type>(piece_index)};
        piece_value(piece.type()) = String::to_number<double>(properties.at(std::string(1, piece.fen_symbol())));
    }

    renormalize_values();
}

void Piece_Strength_Gene::gene_specific_mutation() noexcept
{
    Random::random_element(piece_strength) += Random::random_laplace(0.005);
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
    return 8*std::abs(piece_value(Piece_Type::PAWN)) +
           2*std::abs(piece_value(Piece_Type::ROOK)) +
           2*std::abs(piece_value(Piece_Type::KNIGHT)) +
           2*std::abs(piece_value(Piece_Type::BISHOP)) +
           1*std::abs(piece_value(Piece_Type::QUEEN)) +
           1*std::abs(piece_value(Piece_Type::KING));
}

double Piece_Strength_Gene::file_normalization() const noexcept
{
    return normalization() - std::abs(piece_value(Piece_Type::KING))
                           - 8*std::abs(piece_value(Piece_Type::PAWN));
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

std::string Piece_Strength_Gene::name() const noexcept
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, Piece_Color, size_t, double) const noexcept
{
    return 0.0;
}

double Piece_Strength_Gene::game_progress(const Board& board) const noexcept
{
    const auto king_value = std::abs(piece_value(Piece_Type::KING));
    std::array<double, 2> piece_value_left{};
    for(auto square : Square::all_squares())
    {
        if(const auto piece = board.piece_on_square(square))
        {
            piece_value_left[static_cast<size_t>(piece.color())] += std::abs(piece_value(piece));
        }
    }

    return (1.0 - *std::min_element(piece_value_left.begin(), piece_value_left.end()))/(1.0 - king_value);
}
