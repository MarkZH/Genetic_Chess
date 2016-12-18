#ifndef ILLEGAL_MOVE_EXCEPTION_H
#define ILLEGAL_MOVE_EXCEPTION_H

#include <exception>
#include <string>


class Illegal_Move_Exception : public std::exception
{
    public:
        explicit Illegal_Move_Exception(const std::string& reason);
        ~Illegal_Move_Exception() override;

        const char * what() const noexcept override;

    private:
        std::string message;
};

#endif // ILLEGAL_MOVE_EXCEPTION_H
