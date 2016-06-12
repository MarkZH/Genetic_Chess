#ifndef CHECKMATE_EXCEPTION_H
#define CHECKMATE_EXCEPTION_H

#include "Game_Ending_Exception.h"

#include <string>

enum Color;


class Checkmate_Exception : public Game_Ending_Exception
{
    public:
        explicit Checkmate_Exception(Color victor);
        virtual ~Checkmate_Exception() override;

        virtual const char* what() const throw() override;

    private:
        std::string message;
};

#endif // CHECKMATE_EXCEPTION_H
