import itertools
import numpy as np
from typing import TextIO
from delete_comments import delete_comments

picture_file_args = {'dpi': 600,
                     'format': 'png'}

plot_params = {'legend text size': 'x-small',
               'scatter dot size': 1,
               'plot line weight': 1,
               'x-axis weight': 0.5,
               'x-axis color': 'k',
               'bar color': 'k'}


def moving_mean(x: np.array, window: int) -> np.array:
    x_avg = np.convolve(x, np.ones(window)/window, mode="valid")
    return x_avg


def centered_x_axis(x: np.array, y: np.array) -> np.array:
    left_margin = (len(x) - len(y)) // 2
    right_margin = len(x) - len(y) - left_margin
    return x[left_margin : -right_margin or len(x)]


class No_More_Games(Exception):
    pass


class Game_Record:
    def __init__(self, input_stream: TextIO) -> None:
        self.headers: dict[str, str] = {}
        self.moves: list[str] = []

        for line in map(str.strip, input_stream):
            if not line:
                continue

            if not line.startswith("["):
                self.moves = game_moves(input_stream, line)
                break

            name, value = line.split(maxsplit=1)
            self.headers[name[1:]] = value.split('"')[1]

        if not self.has_game():
            raise No_More_Games()

    def has_game(self) -> bool:
        return bool(self.headers or self.moves)


def game_moves(input: TextIO, previous_line: str = "") -> list[str]:
    game_lines: list[str] = []
    for line in itertools.chain([previous_line], map(str.strip, input)):
        line = line.split(";", maxsplit=1)[0].strip()
        if not line:
            if not game_lines:
                continue
            game_text = delete_comments(" ".join(game_lines))
            return list(filter(lambda s: "." not in s, game_text.split()))[:-1]
        else:
            game_lines.append(line)
    return []


def read_all_games(game_file_name: str) -> list[Game_Record]:
    game_list = []
    with open(game_file_name) as input:
        try:
            while True:
                game_list.append(Game_Record(input))
        except No_More_Games:
            return game_list
