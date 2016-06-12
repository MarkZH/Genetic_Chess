#ifndef STALEMATE_EXCEPTION_H
#define STALEMATE_EXCEPTION_H

#include "Game_Ending_Exception.h"

#include <string>


class Stalemate_Exception : public Game_Ending_Exception
{
    public:
        explicit Stalemate_Exception(std::string message);
        virtual ~Stalemate_Exception() override;

        virtual const char* what() const throw() override;

    private:
        std::string reason; // 50 moves, 3-fold repeat, pieces, etc.
};

#endif // STALEMATE_EXCEPTION_H
