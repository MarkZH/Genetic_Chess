#!/usr/bin/python

import itertools
from collections import defaultdict
from common import Game_Record

# Count how many times a castling move was picked (O-O = kingside, O-O-O = queenside)


def delete_checkmarks(move: str) -> str:
    return move.replace("+", "").replace("#", "")


def count_castles(all_games: list[Game_Record], color: str) -> None:
    castle_count: dict[str, int] = defaultdict(int)
    for game in all_games:
        for ply in itertools.batched(game.moves, 2):
            white_move = delete_checkmarks(ply[0])
            black_move = delete_checkmarks(ply[1]) if len(ply) == 2 else ""

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


def count_all_castles(all_games: list[Game_Record]):
    for color in ["", "White", "Black"]:
        count_castles(all_games, color)
    print(f"out of {len(all_games)} games")
