#include "Exceptions/Out_Of_Time_Exception.h"

#include <string>

#include "Game/Color.h"

Out_Of_Time_Exception::Out_Of_Time_Exception(Color loser) :
    Game_Ending_Exception(opposite(loser),
                          (loser == BLACK ? std::string("1-0") : std::string("0-1")) + " Time Forfeiture")
{
}

Out_Of_Time_Exception::~Out_Of_Time_Exception()
{
}
