#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>
#include <memory>

#include "Player.h"

#include "Game/Color.h"

// For interfacing with outside programs
class Outside_Player : public Player
{
    public:
        ~Outside_Player() override;

        virtual Color get_ai_color() const = 0;

    protected:
        static void send_command(const std::string& cmd);
        static std::string receive_command();

        virtual std::string receive_move() const = 0;

        static void log(const std::string& data);

    private:
        static std::string log_file_name;

        friend std::unique_ptr<Outside_Player> connect_to_outside();
};

std::unique_ptr<Outside_Player> connect_to_outside();

#endif // OUTSIDE_PLAYER_H
