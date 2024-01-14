#!/usr/bin/python

import itertools
from collections import defaultdict
import numpy as np
import matplotlib.pyplot as plt
from common import Game_Record, print_sorted_count_table, picture_file_args

# Count how many times a castling move was picked (O-O = kingside, O-O-O = queenside)


def delete_checkmarks(move: str) -> str:
    return move.replace("+", "").replace("#", "")


def count_castles(all_games: list[Game_Record], color: str, ax: plt.Axes) -> None:
    castle_count: dict[str, int] = defaultdict(int)
    games_where_castled = np.zeros_like(all_games, dtype=int)
    for index, game in enumerate(all_games):
        for ply in itertools.batched(game.moves, 2):
            white_move = delete_checkmarks(ply[0])
            black_move = delete_checkmarks(ply[1]) if len(ply) == 2 else ""

            white_castled = "O" in white_move
            black_castled = "O" in black_move

            if color != "White" and black_castled:
                castle_count[black_move] += 1
            if color != "Black" and white_castled:
                castle_count[white_move] += 1

            if color == "White" and white_castled:
                games_where_castled[index] = 1
            elif color == "Black" and black_castled:
                games_where_castled[index] = 1

    castle_count["Total"] = sum(castle_count.values())
    print("\n" + f"# {color}".strip() + " Castling")
    print_sorted_count_table(castle_count.items())

    if color:
        ax.plot(np.arange(1, len(all_games) + 1), np.cumsum(games_where_castled), label=color)


def count_all_castles(all_games: list[Game_Record], game_file_name: str):
    fig, ax = plt.subplots()
    for color in ["", "White", "Black"]:
        count_castles(all_games, color, ax)
    print(f"out of {len(all_games)} games")

    ax.set_title("Total number of castles")
    ax.set_ylabel("Cumulative castle count")
    ax.set_xlabel("Game count")
    ax.legend()
    fig.savefig(f"{game_file_name}_castle_counts.{picture_file_args['format']}", **picture_file_args)
