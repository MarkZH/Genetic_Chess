#ifndef SQUARE_H
#define SQUARE_H

class Square
{
    public:
        Square();
        Square(char file_in, int rank_in);

        char file() const;
        int rank() const;

        operator bool() const;

    private:
        char square_file;
        int square_rank;
};

// The distance between two squares in king moves
int king_distance(const Square& a, const Square& b);

bool operator==(const Square& a, const Square& b);
bool operator!=(const Square& a, const Square& b);
bool operator<(const Square& a, const Square& b);

#endif // SQUARE_H
