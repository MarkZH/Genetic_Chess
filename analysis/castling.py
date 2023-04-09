#!/usr/bin/python

import sys
from collections import defaultdict
from typing import Dict
from delete_comments import delete_comments

# Count how many times a castling move was picked (O-O = kingside, O-O-O = queenside)


def delete_checkmarks(move: str) -> str:
    return move.replace("+", "").replace("#", "")


def count_castles(game_file_name: str, color: str) -> None:
    castle_count: Dict[str, int] = defaultdict(lambda: 0)
    with open(game_file_name) as game_file:
        for line in game_file:
            if "O" not in line:
                continue
            line = delete_comments(line)
            parts = line.split()
            white_move = parts[1] if len(parts) > 1 else ""
            black_move = parts[2] if len(parts) > 2 else ""

            white_move = delete_checkmarks(white_move)
            black_move = delete_checkmarks(black_move)

            if color != "White" and "O" in black_move:
                castle_count[black_move] += 1
            if color != "Black" and "O" in white_move:
                castle_count[white_move] += 1

    castle_count["Total"] = sum(castle_count.values())
    print("\n" + f"# {color}".strip() + " Castling")
    count_column_width = len(str(max(castle_count.values())))
    for castle, count in sorted(castle_count.items(), key=lambda x: x[0]):
        spaces = " "*(count_column_width - len(str(count)))
        print(spaces, count, castle)


def count_all_castles(game_file):
    for color in ["", "White", "Black"]:
        count_castles(game_file, color)

    with open(game_file) as games:
        game_count = sum(1 for line in games if "Round" in line)
    print(f"out of {game_count} games")


if __name__ == "__main__":
    count_all_castles(sys.argv[1])
