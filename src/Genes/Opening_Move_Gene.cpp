#include "Genes/Opening_Move_Gene.h"

#include <string>
#include <map>
#include <vector>

#include "Game/Board.h"
#include "Moves/Move.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"

#include "Utility/Random.h"

namespace
{
    // Since the first white move has no effect on black's legal first moves,
    // the board for black's first move can be generated once.
    Board second_move_board()
    {
        auto board = Board();
        board.play_move(*board.legal_moves().front());
        return board;
    }
}

Opening_Move_Gene::Opening_Move_Gene() noexcept
{
    opening_move_choice[nullptr] = nullptr;
    static const auto board = Board();
    for(const auto move : board.legal_moves())
    {
        opening_move_choice[move] = nullptr;
    }
}

std::string Opening_Move_Gene::name() const noexcept
{
    return "Opening Move Gene";
}

const Move* Opening_Move_Gene::first_move_choice(const Board& board) const noexcept
{
    if( ! board.started_in_standard_position() || board.all_ply_count() > 1)
    {
        return nullptr;
    }

    return opening_move_choice.at(board.last_move());
}

double Opening_Move_Gene::score_board(const Board&, Piece_Color, size_t, double) const noexcept
{
    return 0.0;
}

void Opening_Move_Gene::gene_specific_mutation() noexcept
{
    const auto index = Random::random_integer<size_t>(0, opening_move_choice.size() - 1);
    const auto entry = std::next(opening_move_choice.begin(), index);
 
    static const auto board = Board();
    auto move_list = board.legal_moves();
    if(entry->first)
    {
        static const auto second_board = second_move_board();
        move_list = second_board.legal_moves();
    }

    move_list.push_back(nullptr);
    entry->second = Random::random_element(move_list);
}

void Opening_Move_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    delete_priorities(properties);

    static const auto board = Board();
    for(const auto& [left_move, right_move] : opening_move_choice)
    {
        auto key = std::string{"Start"};
        auto value = std::string{"-"};
        if(left_move)
        {
            key = left_move->algebraic(board);
            if(right_move)
            {
                static const auto second_board = second_move_board();
                value = right_move->algebraic(second_board);
            }
        }
        else if(right_move)
        {
            value = right_move->algebraic(board);
        }
        properties[key] = value;
    }
}

void Opening_Move_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    static const auto board = Board();
    for(const auto& [first_move_text, second_move_text] : properties)
    {
        const auto first_move = (first_move_text == "Start" ? nullptr : &board.interpret_move(first_move_text));
        if(second_move_text == "-")
        {
            opening_move_choice[first_move] = nullptr;
        }
        else
        {
            if(first_move)
            {
                static const auto second_board = second_move_board();
                opening_move_choice[first_move] = &second_board.interpret_move(second_move_text);
            }
            else
            {
                opening_move_choice[first_move] = &board.interpret_move(second_move_text);
            }
        }
    }
}
