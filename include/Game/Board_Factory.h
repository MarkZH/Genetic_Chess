#ifndef BOARD_FACTORY_H
#define BOARD_FACTORY_H

#include <memory>
#include <string>

class Board;

std::unique_ptr<Board> board_factory(const std::string& fen);

#endif // BOARD_FACTORY_H
