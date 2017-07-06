#include "Game/Square.h"

#include <cmath>
#include <algorithm>

bool Square::operator<(const Square& other) const
{
    if(file == other.file)
    {
        return rank < other.rank;
    }
    return file < other.file;
}

Square::operator bool() const
{
    return file != '\0' && rank != 0;
}

int king_distance(const Square& a, const Square& b)
{
    return std::max(std::abs(a.file - b.file),
                    std::abs(a.rank - b.rank));
}
