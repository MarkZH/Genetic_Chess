#ifndef SQUARE_H
#define SQUARE_H

#include <memory>

class Piece;

class Square
{
    public:
        Square();

        std::shared_ptr<const Piece> piece_on_square() const;
        std::shared_ptr<const Piece> piece_on_square();
        void place_piece(const std::shared_ptr<const Piece>& p);

        bool is_en_passant_targetable() const;
        void make_en_passant_targetable();
        void remove_en_passant_targetable();

        bool piece_has_moved() const;
        void player_moved_piece();

        bool empty() const;

    private:
        std::shared_ptr<const Piece> piece_on_this_square;
        bool is_en_passant_target;
        bool piece_moved;
};

#endif // SQUARE_H
