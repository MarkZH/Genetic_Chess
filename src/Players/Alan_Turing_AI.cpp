#include "Players/Alan_Turing_AI.h"

#include <vector>
#include <cmath>

#include "Game/Board.h"
#include "Moves/Move.h"
#include "Game/Piece.h"
#include "Game/Game_Result.h"
#include "Game/Color.h"
#include "Moves/Threat_Generator.h"

const Move& Alan_Turing_AI::choose_move(const Board& board, const Clock&) const
{
    // Every possible first move is considerable
    std::pair<double, double> best_first_move_score = {-1001.0, -1001.0}; // maximize
    auto best_first_move = board.legal_moves().front();
    for(auto first_move : board.legal_moves())
    {
        auto first_board = board;
        auto first_move_result = first_board.submit_move(*first_move);
        std::pair<double, double> first_move_score;
        if(first_move_result.game_has_ended())
        {
            first_move_score = position_value(first_board, board.whose_turn(), first_move_result);
        }
        else
        {
            // Every possible reply is considerable
            std::pair<double, double> worst_second_move_score = {1001.0, 1001.0}; // minimize
            for(auto second_move : first_board.legal_moves())
            {
                auto second_board = first_board;
                std::pair<double, double> second_move_score;
                auto second_move_result = second_board.submit_move(*second_move);
                if(second_move_result.game_has_ended())
                {
                    second_move_score = position_value(second_board, board.whose_turn(), second_move_result);
                }
                else
                {
                    std::pair<double, double> best_third_move_score = {-1001.0, -1001.0}; // maximize
                    for(auto third_move : second_board.legal_moves())
                    {
                        auto third_board = second_board;
                        auto third_move_result = third_board.submit_move(*third_move);
                        auto third_move_score = position_value(third_board, board.whose_turn(), third_move_result);
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

std::string Alan_Turing_AI::name() const
{
    return "Turbocomp";
}

std::string Alan_Turing_AI::author() const
{
    return "Alan Turing";
}

std::vector<const Move*> Alan_Turing_AI::considerable_moves(const Board& board) const
{
    std::vector<const Move*> result;
    for(auto move : board.legal_moves())
    {
        if(is_considerable(*move, board))
        {
            result.push_back(move);
        }
    }

    return result;
}

bool Alan_Turing_AI::is_considerable(const Move& move, const Board& board) const
{
    // Recapture is considerable
    if(board.last_move_captured() && board.move_captures(move))
    {
        auto last_move = board.game_record().back();
        if(last_move->end_file() == move.end_file() && last_move->end_rank() == move.end_rank())
        {
            return true;
        }
    }

    auto attacking_piece = board.piece_on_square(move.start_file(), move.start_rank());
    auto attacked_piece = board.piece_on_square(move.end_file(), move.end_rank());
    if(attacked_piece)
    {
        // Capturing an undefended piece is considerable
        auto temp_board = board;
        auto result = temp_board.submit_move(move);
        if(temp_board.safe_for_king(move.end_file(), move.end_rank(), attacking_piece->color()))
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

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece)
            {
                if(piece->color() == perspective)
                {
                    player_score += piece_value(piece);
                }
                else
                {
                    opponent_score += piece_value(piece);
                }
            }
        }
    }

    return player_score/opponent_score;
}

double Alan_Turing_AI::piece_value(const Piece* piece) const
{
    if(!piece)
    {
        return 0.0;
    }

    //                        P    R    N    B    Q     K
    static double values[] = {1.0, 5.0, 3.0, 3.5, 10.0, 0.0};
    return values[piece->type()];
}

double Alan_Turing_AI::position_play_value(const Board& board, Color perspective) const
{
    double total_score = 0.0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if( ! piece)
            {
                continue;
            }

            if(piece->color() == perspective)
            {
                if(piece->type() == QUEEN || piece->type() == ROOK || piece->type() == BISHOP || piece->type() == KNIGHT)
                {
                    // Number of moves score
                    double move_score = 0.0;
                    for(auto move : board.legal_moves())
                    {
                        if(move->start_file() == file && move->start_rank() == rank)
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
                    if(piece->type() != QUEEN)
                    {
                        Square defending_square{};
                        for(auto defender : Threat_Generator(file, rank, opposite(perspective), board))
                        {
                            if(defending_square)
                            {
                                total_score += 0.5;
                            }
                            else
                            {
                                total_score += 1.0;
                                defending_square = defender;
                            }
                        }
                    }
                }
                else if(piece->type() == KING)
                {
                    // King move scores
                    double move_score = 0.0;
                    double castling_moves = 0.0;
                    for(auto move : board.legal_moves())
                    {
                        if(move->start_file() != file || move->start_rank() != rank)
                        {
                            continue;
                        }

                        if(move->is_castling())
                        {
                            castling_moves += 1.0;
                        }
                        else
                        {
                            move_score += 1.0;
                        }
                    }
                    total_score += std::sqrt(move_score) + castling_moves;

                    // King vulnerability (count number of queen moves from king square and subtract)
                    double king_squares = 0.0;
                    for(int file_step = -1; file_step <= 1; ++file_step)
                    {
                        for(int rank_step = -1; rank_step <= 1; ++rank_step)
                        {
                            if(file_step == 0 && rank_step == 0)
                            {
                                continue;
                            }

                            for(int steps = 1; steps <= 7; ++steps)
                            {
                                char attack_file = file + steps*file_step;
                                int  attack_rank = rank + steps*rank_step;

                                if(!board.inside_board(attack_file, attack_rank))
                                {
                                    break;
                                }

                                auto other_piece = board.piece_on_square(attack_file, attack_rank);
                                if( ! other_piece)
                                {
                                    king_squares += 1.0;
                                }
                                else
                                {
                                    if(other_piece->color() != perspective)
                                    {
                                        king_squares += 1.0;
                                    }

                                    break;
                                }
                            }
                        }
                    }
                    total_score -= std::sqrt(king_squares);

                    // Castling score
                    if(!board.piece_has_moved(file, rank))
                    {
                        // Queenside castling
                        if( ! board.piece_has_moved('a', rank))
                        {
                            total_score += 1.0;

                            // Can castle on next move
                            if(board.all_empty_between('a', rank, file, rank))
                            {
                                total_score += 1.0;
                            }
                        }

                        // Kingside castling
                        if(!board.piece_has_moved('h', rank))
                        {
                            total_score += 1.0;

                            // Can castle on next move
                            if(board.all_empty_between('h', rank, file, rank))
                            {
                                total_score += 1.0;
                            }
                        }
                    }

                    // Last move was castling
                    if(board.castling_move_index(perspective) < board.game_record().size())
                    {
                        total_score += 1.0;
                    }
                }
                else if(piece->type() == PAWN)
                {
                    // Pawn advancement
                    auto base_rank = (perspective == WHITE ? 2 : 7);
                    total_score += 0.2*std::abs(base_rank - rank);

                    // Pawn defended
                    auto pawn_defended = false;
                    for(auto piece_type :{QUEEN, ROOK, BISHOP, KNIGHT, KING})
                    {
                        if(pawn_defended)
                        {
                            break;
                        }

                        auto defending_piece = board.piece_instance(piece_type, perspective);
                        for(auto move : defending_piece->move_list(file, rank))
                        {
                            if(defending_piece == board.piece_on_square(move->end_file(), move->end_rank()))
                            {
                                if(piece_type == KNIGHT || board.all_empty_between(file, rank, move->end_file(), move->end_rank()))
                                {
                                    pawn_defended = true;
                                    break;
                                }
                            }
                        }
                    }

                    if(pawn_defended)
                    {
                        total_score += 0.3;
                    }
                }
            }
            else // piece->color() == opposite(perspective)
            {
                if(piece->type() == KING)
                {
                    auto temp_board = board;
                    temp_board.set_turn(opposite(perspective));
                    if(temp_board.king_is_in_check())
                    {
                        total_score += 0.5;
                    }
                    else
                    {
                        temp_board.set_turn(perspective);
                        for(auto move : temp_board.legal_moves())
                        {
                            auto temp_temp_board = temp_board;
                            if(temp_temp_board.submit_move(*move).winner() != NONE)
                            {
                                total_score += 1.0;
                            }
                        }
                    }
                }
            }
        }
    }

    return total_score;
}

std::pair<double, double> Alan_Turing_AI::position_value(const Board& board, Color perspective, const Game_Result& move_result) const
{
    auto best_score = score_board(board, perspective, move_result);
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
            best_score = std::max(best_score, score_board(temp_board, perspective, result));
        }
    }

    return best_score;
}

std::pair<double, double> Alan_Turing_AI::score_board(const Board& board, Color perspective, const Game_Result& move_result) const
{
    if(move_result.game_has_ended())
    {
        if(move_result.winner() == perspective)
        {
            return {1000.0, 1000.0};
        }
        else if(move_result.winner() == opposite(perspective))
        {
            return {-1000.0, -1000.0};
        }
        else
        {
            return {0.0, 0.0};
        }
    }

    return std::make_pair(material_value(board, perspective), position_play_value(board, perspective));
}
