#ifndef SQUARE_H
#define SQUARE_H

struct Square
{
    char file;
    int rank;

    operator bool() const;
};

// The distance between two squares in king moves
int king_distance(const Square& a, const Square& b);

#endif // SQUARE_H
