#include "Players/Alan_Turing_AI.h"

#include <vector>
#include <cmath>
#include <bitset>
#include <tuple>
#include <algorithm>
#include <numeric>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Game_Result.h"
#include "Game/Color.h"
#include "Game/Square.h"

#include "Moves/Move.h"

//! Turing's algorithm is a depth-3 minimax algorithm with an complex evalutation function.
//
//! The evaluation function is especially complex given that it had to be run on pencil and paper.
const Move& Alan_Turing_AI::choose_move(const Board& board, const Clock&) const
{
    // Every possible first move is considerable
    const auto worst_score_start = std::make_tuple(1001.0, 1001.0, 0);
    const auto best_score_start  = std::make_tuple(-1001.0, -1001.0, 0);

    auto best_first_move_score = best_score_start; // maximize
    auto best_first_move = board.legal_moves().front();
    for(auto first_move : board.legal_moves())
    {
        auto first_board = board;
        auto first_move_result = first_board.submit_move(*first_move);
        std::tuple<double, double, int> first_move_score;
        if(first_move_result.game_has_ended())
        {
            first_move_score = position_value(first_board, board.whose_turn(), first_move_result, 1);
        }
        else
        {
            // Every possible reply is considerable
            auto worst_second_move_score = worst_score_start; // minimize
            for(auto second_move : first_board.legal_moves())
            {
                auto second_board = first_board;
                std::tuple<double, double, int> second_move_score;
                auto second_move_result = second_board.submit_move(*second_move);
                if(second_move_result.game_has_ended())
                {
                    second_move_score = position_value(second_board, board.whose_turn(), second_move_result, 2);
                }
                else
                {
                    auto best_third_move_score = best_score_start; // maximize
                    for(auto third_move : second_board.legal_moves())
                    {
                        if(board.must_pick_move_now())
                        {
                            return *best_first_move;
                        }

                        auto third_board = second_board;
                        auto third_move_result = third_board.submit_move(*third_move);
                        auto third_move_score = position_value(third_board, board.whose_turn(), third_move_result, 3);
                        best_third_move_score = std::max(best_third_move_score, third_move_score);
                    }

                    second_move_score = best_third_move_score;
                }

                worst_second_move_score = std::min(worst_second_move_score, second_move_score);
            }

            first_move_score = worst_second_move_score;
        }

        if(first_move_score > best_first_move_score)
        {
            best_first_move = first_move;
            best_first_move_score = first_move_score;
        }
    }

    return *best_first_move;
}

//! This program was named Turochamp after its creators: Turing and David Champernowne.
//
//! \returns "Turochamp"
std::string Alan_Turing_AI::name() const
{
    return "Turochamp";
}

//! Credit to Turing and Champerowne.
//
//! \returns "Alan Turing and David Champernowne"
std::string Alan_Turing_AI::author() const
{
    return "Alan Turing and David Champernowne";
}

std::vector<const Move*> Alan_Turing_AI::considerable_moves(const Board& board) const
{
    std::vector<const Move*> result;
    std::copy_if(board.legal_moves().begin(), board.legal_moves().end(), std::back_inserter(result),
                 [this, &board](auto move){ return is_considerable(*move, board); });
    return result;
}

bool Alan_Turing_AI::is_considerable(const Move& move, const Board& board) const
{
    // Recapture is considerable
    if(board.last_move_captured() && board.move_captures(move))
    {
        auto last_move = board.game_record().back();
        if(last_move->end() == move.end())
        {
            return true;
        }
    }

    auto attacking_piece = board.piece_on_square(move.start());
    auto attacked_piece = board.piece_on_square(move.end());
    if(attacked_piece)
    {
        // Capturing an undefended piece is considerable
        auto temp_board = board;
        auto result = temp_board.submit_move(move);
        if(temp_board.safe_for_king(move.end(), attacking_piece.color()))
        {
            return true;
        }

        // Capturing with a less valuable piece is considerable
        if(piece_value(attacked_piece) > piece_value(attacking_piece))
        {
            return true;
        }

        // A move resulting in checkmate is considerable
        if(result.winner() == board.whose_turn())
        {
            return true;
        }
    }

    return false;
}

double Alan_Turing_AI::material_value(const Board& board, Color perspective) const
{
    double player_score = 0.0;
    double opponent_score = 0.0;

    for(auto square : Square::all_squares())
    {
        auto piece = board.piece_on_square(square);
        if(piece)
        {
            if(piece.color() == perspective)
            {
                player_score += piece_value(piece);
            }
            else
            {
                opponent_score += piece_value(piece);
            }
        }
    }

    return player_score/opponent_score;
}

double Alan_Turing_AI::piece_value(Piece piece) const
{
    if( ! piece)
    {
        return 0.0;
    }

    //                        P    R    N    B    Q     K
    static double values[] = {1.0, 5.0, 3.0, 3.5, 10.0, 0.0};
    return values[piece.type()];
}

double Alan_Turing_AI::position_play_value(const Board& board, Color perspective) const
{
    double total_score = 0.0;

    for(auto square : Square::all_squares())
    {
        auto piece = board.piece_on_square(square);
        if( ! piece)
        {
            continue;
        }

        if(piece.color() == perspective)
        {
            if(piece.type() == QUEEN || piece.type() == ROOK || piece.type() == BISHOP || piece.type() == KNIGHT)
            {
                // Number of moves score
                double move_score = 0.0;
                for(auto move : board.legal_moves())
                {
                    if(move->start() == square)
                    {
                        move_score += 1.0;
                        if(board.move_captures(*move))
                        {
                            move_score += 1.0;
                        }
                    }
                }
                total_score += std::sqrt(move_score);

                // Non-queen pieces defended
                if(piece.type() != QUEEN)
                {
                    auto defender_count = board.moves_attacking_square(square, perspective).count();
                    if(defender_count > 0)
                    {
                        total_score += 1.0 + 0.5*(defender_count - 1);
                    }
                }
            }
            else if(piece.type() == KING)
            {
                // King move scores
                double move_score = 0.0;
                double castling_moves = 0.0;
                for(auto move : board.legal_moves())
                {
                    if(move->start() != square)
                    {
                        continue;
                    }

                    if(move->is_castling())
                    {
                        castling_moves = 1.0;
                    }
                    else
                    {
                        move_score += 1.0;
                    }
                }
                total_score += std::sqrt(move_score) + castling_moves;

                // King vulnerability (count number of queen moves from king square and subtract)
                double king_squares = 0.0;
                const Move* blocked_attack = nullptr;
                for(auto attack : Piece{perspective, QUEEN}.attacking_moves(square))
                {
                    if(blocked_attack && same_direction(attack->movement(), blocked_attack->movement()))
                    {
                        continue;
                    }
                    else
                    {
                        blocked_attack = nullptr;
                    }

                    auto other_piece = board.piece_on_square(attack->end());
                    if( ! other_piece)
                    {
                        king_squares += 1.0;
                    }
                    else
                    {
                        if(other_piece.color() != perspective)
                        {
                            king_squares += 1.0;
                        }

                        blocked_attack = attack;
                    }
                }
                total_score -= std::sqrt(king_squares);

                // Castling score
                if( ! board.piece_has_moved(square))
                {
                    for(auto rook_file : {'a', 'h'})
                    {
                        auto rook_square = Square{rook_file, square.rank()};
                        if( ! board.piece_has_moved(rook_square))
                        {
                            total_score += 1.0;
                            break;
                        }
                    }
                }

                // Last move was castling
                auto castling_index_check = board.game_record().size() - (perspective == board.whose_turn() ? 2 : 1);
                if(board.game_record().size() > castling_index_check &&
                   board.castling_move_index(perspective) == castling_index_check)
                {
                    total_score += 1.0;
                }
            }
            else if(piece.type() == PAWN)
            {
                // Pawn advancement
                auto base_rank = (perspective == WHITE ? 2 : 7);
                total_score += 0.2*std::abs(base_rank - square.rank());

                // Pawn defended by non-pawns
                auto defender_count = board.moves_attacking_square(square, perspective).count();
                auto rank_change = perspective == WHITE ? -1 : 1;
                for(auto file_change : {-1, 1})
                {
                    auto new_square = square + Square_Difference{file_change, rank_change};
                    if(new_square.inside_board())
                    {
                        if(board.piece_on_square(new_square) == piece)
                        {
                            --defender_count;
                        }
                    }
                }

                if(defender_count > 0)
                {
                    total_score += 0.3;
                }
            }
        }
        else // piece->color() == opposite(perspective)
        {
            if(piece.type() == KING)
            {
                auto opponent = opposite(perspective);
                if( ! board.safe_for_king(board.find_king(opponent), opponent))
                {
                    total_score += 0.5;
                }
                else
                {
                    total_score += std::count_if(board.legal_moves().begin(),
                                                 board.legal_moves().end(),
                                                 [board](auto move)
                                                 {
                                                     auto new_board = board;
                                                     return new_board.submit_move(*move).winner() != NONE;
                                                 });
                }
            }
        }
    }

    return total_score;
}

std::tuple<double, double, int> Alan_Turing_AI::position_value(const Board& board,
                                                               Color perspective,
                                                               const Game_Result& move_result,
                                                               int depth) const
{
    auto best_score = score_board(board, perspective, move_result, depth);
    if(move_result.game_has_ended())
    {
        return best_score;
    }

    auto considerable_move_list = considerable_moves(board);

    // Skip if every move is considerable
    if(considerable_move_list.size() < board.legal_moves().size())
    {
        for(auto move : considerable_moves(board))
        {
            auto temp_board = board;
            auto result = temp_board.submit_move(*move);
            best_score = std::max(best_score, score_board(temp_board, perspective, result, depth + 1));
        }
    }

    return best_score;
}

std::tuple<double, double, int> Alan_Turing_AI::score_board(const Board& board,
                                                            Color perspective,
                                                            const Game_Result& move_result,
                                                            int depth) const
{
    if(move_result.game_has_ended())
    {
        if(move_result.winner() == perspective)
        {
            // An earlier win is preferable to a later win,
            // so use the negative value of the depth.
            return {1000.0, 1000.0, -depth};
        }
        else if(move_result.winner() == opposite(perspective))
        {
            // A later loss is preferable to an earlier loss,
            // so use the positive value of the depth.
            return {-1000.0, -1000.0, depth};
        }
        else
        {
            // Draw depth does not matter.
            return {0.0, 0.0, 0};
        }
    }

    // Depth does not matter for other moves.
    return {material_value(board, perspective), position_play_value(board, perspective), 0};
}
