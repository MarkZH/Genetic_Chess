#ifndef GAME_RESULT_H
#define GAME_RESULT_H

#include <string>

#include "Color.h"

enum Game_Result_Type
{
    ONGOING,
    CHECKMATE,
    STALEMATE,
    FIFTY_MOVE,
    THREEFOLD_REPETITION,
    INSUFFICIENT_MATERIAL,
    TIME_FORFEIT
};

//! This class represents the result of a game action, primarily whether the game is still ongoing.
class Game_Result
{
    public:
        Game_Result();
        Game_Result(Color winner, Game_Result_Type reason);

        bool game_has_ended() const;
        Color winner() const;
        std::string ending_reason() const;
        std::string game_ending_annotation() const;
        std::string game_record_annotation() const;

    private:
        Color victor;
        Game_Result_Type cause;
};

#endif // GAME_RESULT_H
