#include "Game/Board_Factory.h"

#include <memory>
#include <string>

#include "Game/Board.h"
#include "Game/Musketeer_Board.h"

#include "Utility/String.h"

std::unique_ptr<Board> board_factory(const std::string& fen)
{
    if(String::contains(fen, "*"))
    {
        return std::make_unique<Musketeer_Board>(fen);
    }
    else
    {
        return std::make_unique<Board>(fen);
    }
}