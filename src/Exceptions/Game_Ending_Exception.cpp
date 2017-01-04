#include "Exceptions/Game_Ending_Exception.h"

Game_Ending_Exception::Game_Ending_Exception(Color winner_in,
                                             const std::string& message_in) :
    victor(winner_in),
    message(message_in)
{
}

Game_Ending_Exception::~Game_Ending_Exception()
{
}

Color Game_Ending_Exception::winner() const
{
    return victor;
}

const char* Game_Ending_Exception::what() const throw()
{
    return message.c_str();
}
