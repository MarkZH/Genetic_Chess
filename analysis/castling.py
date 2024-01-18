#!/usr/bin/python

from collections import defaultdict
from enum import StrEnum, auto
import numpy as np
import matplotlib.pyplot as plt
from common import read_all_games, print_sorted_count_table, picture_file_args, moving_mean, centered_x_axis


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

    cumulative_figure, cumulative_axes = plt.subplots()
    rate_figure, rate_axes = plt.subplots()
    for color in Color:
        castling_games = np.array(games_where_castled[color], dtype=int)
        cumulative_castles = np.cumsum(castling_games)
        x_axis = np.arange(cumulative_castles.size) + 1
        cumulative_axes.plot(x_axis, cumulative_castles, label=color.title())

        castling_rate = 100*moving_mean(castling_games, 10000)
        rate_x_axis = centered_x_axis(x_axis, castling_rate)
        rate_axes.plot(rate_x_axis, castling_rate, label=color.title())

    cumulative_axes.set_title("Total number of castles")
    cumulative_axes.set_ylabel("Cumulative castle count")
    cumulative_axes.set_xlabel("Game count")
    cumulative_axes.legend()
    cumulative_figure.tight_layout()
    cumulative_figure.savefig(f"{games_file_name}_castle_counts.{picture_file_args['format']}", **picture_file_args)
    plt.close(cumulative_figure)

    rate_axes.set_title("Percent of games with castling")
    rate_axes.set_ylabel("Castling percentage")
    rate_axes.set_xlabel("Game count")
    rate_axes.legend()
    rate_figure.tight_layout()
    rate_figure.savefig(f"{games_file_name}_castle_rate.{picture_file_args['format']}", **picture_file_args)
    plt.close(rate_figure)

    for color in Color:
        castle_count[color]["Total"] = sum(castle_count[color].values())
        print("\n" + f"# {color.title()}".strip() + " castling")
        print_sorted_count_table(castle_count[color].items())
        print(f"\nout of {len(games_where_castled[color])} games")
