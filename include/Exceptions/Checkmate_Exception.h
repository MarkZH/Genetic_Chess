#ifndef CHECKMATE_EXCEPTION_H
#define CHECKMATE_EXCEPTION_H

#include "Game_Ending_Exception.h"

#include <string>

enum Color;

class Checkmate_Exception : public Game_Ending_Exception
{
    public:
        explicit Checkmate_Exception(Color victor);
};

#endif // CHECKMATE_EXCEPTION_H
