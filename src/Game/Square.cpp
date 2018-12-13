#include "Game/Square.h"

#include <cmath>
#include <algorithm>

Square::Square() : square_file('\0'), square_rank(0)
{
}

Square::Square(char file_in, int rank_in) : square_file(file_in), square_rank(rank_in)
{
}

char Square::file() const
{
    return square_file;
}

int Square::rank() const
{
    return square_rank;
}

Square::operator bool() const
{
    return file() != '\0' && rank() != 0;
}

int king_distance(const Square& a, const Square& b)
{
    return std::max(std::abs(a.file() - b.file()),
                    std::abs(a.rank() - b.rank()));
}

bool operator==(const Square& a, const Square& b)
{
    return (a.file() == b.file()) && (a.rank() == b.rank());
}

bool operator!=(const Square& a, const Square& b)
{
    return !(a == b);
}

bool operator<(const Square& a, const Square& b)
{
    if(a.file() < b.file())
    {
        return true;
    }
    else if(b.file() < a.file())
    {
        return false;
    }
    else
    {
        return a.rank() < b.rank();
    }
}
