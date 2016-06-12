#ifndef OUT_OF_TIME_EXCEPTION_H
#define OUT_OF_TIME_EXCEPTION_H

#include "Game_Ending_Exception.h"

#include <string>

#include "Game/Color.h"

class Out_Of_Time_Exception : public Game_Ending_Exception
{
    public:
        explicit Out_Of_Time_Exception(Color loser);
        virtual ~Out_Of_Time_Exception() override;

        const char* what() const throw() override;
    private:
        std::string message;
};

#endif // OUT_OF_TIME_EXCEPTION_H
