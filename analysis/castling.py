#!/usr/bin/python

from collections import defaultdict
from enum import StrEnum, auto
import numpy as np
import matplotlib.pyplot as plt
from common import read_all_games, print_sorted_count_table, picture_file_args


def delete_checkmarks(move: str) -> str:
    return move.rstrip("+").rstrip("#")


class Color(StrEnum):
    WHITE = auto()
    BLACK = auto()


def count_all_castles(games_file_name: str) -> None:
    castle_count: dict[Color, dict[str, int]] = {Color.WHITE: defaultdict(int),
                                                 Color.BLACK: defaultdict(int)}
    games_where_castled: dict[Color, list[bool]] = {Color.WHITE: [],
                                                    Color.BLACK: []}
    for game in read_all_games(games_file_name):
        for color in Color:
            moves = game.moves[::2] if color == Color.WHITE else game.moves[1::2]
            castle = ["O" in move for move in moves]
            castle_index = castle.index(True) if True in castle else -1
            castled = castle_index != -1
            if castled:
                castle_count[color][delete_checkmarks(moves[castle_index])] += 1
            games_where_castled[color].append(castled)

    fig, ax = plt.subplots()
    for color in Color:
        castle_count[color]["Total"] = sum(castle_count[color].values())
        print("\n" + f"# {color.title()}".strip() + " castling")
        print_sorted_count_table(castle_count[color].items())
        print(f"\nout of {len(games_where_castled[color])} games")
        cumulative_castles = np.cumsum(np.array(games_where_castled[color], dtype=int))
        x_axis = np.arange(cumulative_castles.size) + 1
        ax.plot(x_axis, cumulative_castles, label=color.title())

    ax.set_title("Total number of castles")
    ax.set_ylabel("Cumulative castle count")
    ax.set_xlabel("Game count")
    ax.legend()

    fig.tight_layout()
    fig.savefig(f"{games_file_name}_castle_counts.{picture_file_args['format']}", **picture_file_args)
    plt.close(fig)
