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


def game_moves(input: TextIO, previous_line: str = "") -> list[str]:
    game_lines: list[str] = []
    for line in itertools.chain([previous_line], map(str.strip, input)):
        line = line.split(";", maxsplit=1)[0].strip()
        if not line:
            if not game_lines:
                continue
            game_text = delete_comments(" ".join(game_lines))
            return list(filter(lambda s: "." not in s, game_text.split()))[:-1]
        elif line.startswith("["):
            continue
        else:
            game_lines.append(line)
    return []
