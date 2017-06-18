#ifndef GAME_RESULT_H
#define GAME_RESULT_H

#include <string>

#include "Color.h"

class Game_Result
{
    public:
        Game_Result();
        Game_Result(Color winner, const std::string& reason, bool end_by_clock);

        bool game_has_ended() const;
        Color get_winner() const;
        std::string get_ending_reason() const;
        std::string get_game_ending_annotation() const;
        std::string get_game_record_annotation() const;

    private:
        Color victor;
        std::string cause;
        bool out_of_time_ending;
};

#endif // GAME_RESULT_H
