#ifndef STALEMATE_EXCEPTION_H
#define STALEMATE_EXCEPTION_H

#include "Game_Ending_Exception.h"

#include <string>


class Stalemate_Exception : public Game_Ending_Exception
{
    public:
        explicit Stalemate_Exception(const std::string& message_in);
        ~Stalemate_Exception() override;
};

#endif // STALEMATE_EXCEPTION_H
