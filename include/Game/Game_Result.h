#ifndef GAME_RESULT_H
#define GAME_RESULT_H

#include <string>

#include "Color.h"

class Game_Result
{
    public:
        Game_Result();
        Game_Result(Color winner, const std::string& reason);

        bool game_has_ended() const;
        Color winner() const;
        std::string ending_reason() const;
        std::string game_ending_annotation() const;
        std::string game_record_annotation() const;

    private:
        Color victor;
        std::string cause;
};

#endif // GAME_RESULT_H
