#!/usr/bin/python

from collections import defaultdict
from enum import StrEnum, auto
import numpy as np
import matplotlib.pyplot as plt
from common import Game_Record, print_sorted_count_table, picture_file_args


def delete_checkmarks(move: str) -> str:
    return move.replace("+", "").replace("#", "")


class Color(StrEnum):
    WHITE = auto()
    BLACK = auto()


def count_castles(all_games: list[Game_Record], color: Color) -> None:
    castle_count: dict[str, int] = defaultdict(int)
    games_where_castled = np.zeros_like(all_games, dtype=bool)
    for index, game in enumerate(all_games):
        moves = game.moves[::2] if color == Color.WHITE else game.moves[1::2]
        castle = ["O" in move for move in moves]
        castle_index = castle.index(True) if True in castle else -1
        castled = castle_index != -1

        if castled:
            castle_count[delete_checkmarks(moves[castle_index])] += 1
            games_where_castled[index] = True

    castle_count["Total"] = sum(castle_count.values())
    print("\n" + f"# {color}".strip() + " Castling")
    print_sorted_count_table(castle_count.items())
    return games_where_castled


def count_and_plot_castles(color, all_games, ax, x_axis):
    castling_games = count_castles(all_games, color)
    cumulative_castles = np.cumsum(castling_games.astype(int))
    ax.plot(x_axis, cumulative_castles, label=color.title())


def count_all_castles(all_games: list[Game_Record], game_file_name: str):
    fig, ax = plt.subplots()
    x_axis = np.arange(len(all_games)) + 1
    count_and_plot_castles(Color.WHITE, all_games, ax, x_axis)
    count_and_plot_castles(Color.BLACK, all_games, ax, x_axis)

    print(f"\nout of {len(all_games)} games")

    ax.set_title("Total number of castles")
    ax.set_ylabel("Cumulative castle count")
    ax.set_xlabel("Game count")
    ax.legend()

    fig.tight_layout()
    fig.savefig(f"{game_file_name}_castle_counts.{picture_file_args['format']}", **picture_file_args)
