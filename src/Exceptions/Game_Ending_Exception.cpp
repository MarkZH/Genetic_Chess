#include "Exceptions/Game_Ending_Exception.h"

Game_Ending_Exception::Game_Ending_Exception(Color winner_in) : victor(winner_in)
{
}

Game_Ending_Exception::~Game_Ending_Exception()
{
}

Color Game_Ending_Exception::winner() const
{
    return victor;
}
