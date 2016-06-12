#ifndef GENERIC_EXCEPTION_H
#define GENERIC_EXCEPTION_H

#include <exception>
#include <string>


class Generic_Exception : public std::exception
{
    public:
        explicit Generic_Exception(std::string message);
        virtual ~Generic_Exception() override;

        const char* what() const throw() override;

    private:
        std::string error;
};

#endif // GENERIC_EXCEPTION_H
