#include "Genes/Opening_Move_Gene.h"

#include <string>
#include <map>
#include <vector>

#include "Game/Board.h"
#include "Moves/Move.h"
#include "Game/Color.h"

#include "Utility/Random.h"

std::string Opening_Move_Gene::name() const noexcept
{
    return "Opening Move Gene";
}

const Move* Opening_Move_Gene::first_move_choice(const Board& board) const noexcept
{
    if(board.started_in_standard_position() && board.all_ply_count() == 0)
    {
        return opening_move_choice;
    }
    else
    {
        return nullptr;
    }
}

double Opening_Move_Gene::score_board(const Board&, Piece_Color, size_t, double) const noexcept
{
    return 0.0;
}

void Opening_Move_Gene::gene_specific_mutation() noexcept
{
    static const auto board = Board();
    auto move_list = board.legal_moves();
    move_list.push_back(nullptr);
    opening_move_choice = Random::random_element(move_list);
}

void Opening_Move_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    delete_priorities(properties);
    static const auto board = Board();
    properties["Opening Move"] = opening_move_choice ? opening_move_choice->algebraic(board) : "-";
}

void Opening_Move_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    static const auto board = Board();
    const auto move_text = properties.at("Opening Move");
    opening_move_choice = (move_text == "-" ? nullptr : &board.interpret_move(move_text));
}
