import itertools
import numpy as np
import numpy.typing as npt
from typing import TextIO, Iterable, Iterator, Any

picture_file_args = {'dpi': 600,
                     'format': 'png'}

plot_params = {'legend text size': 'x-small',
               'scatter dot size': 1,
               'plot line weight': 1,
               'x-axis weight': 0.5,
               'x-axis color': 'k',
               'bar color': 'k'}


def moving_mean(x: npt.NDArray, window: int) -> npt.NDArray:
    x_avg = np.convolve(x, np.ones(window)/window, mode="valid")
    return x_avg


def centered_x_axis(x: npt.NDArray, y: npt.NDArray) -> npt.NDArray:
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

            if line.startswith("["):
                name, value = line.split(maxsplit=1)
                self.headers[name[1:]] = value.split('"')[1]
            else:
                self.moves = game_moves(input_stream, line)
                break

        if not self.has_game():
            raise No_More_Games()

    def has_game(self) -> bool:
        return bool(self.headers or self.moves)


def game_moves(input: TextIO, previous_line: str = "") -> list[str]:
    game_lines: list[str] = []
    for line in itertools.chain([previous_line], map(str.strip, input)):
        if line:
            game_lines.append(line.split(";", maxsplit=1)[0].strip())
        else:
            if not game_lines:
                continue
            game_text = delete_comments(" ".join(game_lines))
            return list(filter(lambda s: "." not in s, game_text.split()))[:-1]
    return []


def delete_comments(line: str) -> str:
    parentheses_depth = 0
    inside_brace = False
    game_text: list[str] = []
    start_index = 0
    for index, character in enumerate(line):
        if inside_brace:
            if character == "}":
                inside_brace = False
                if parentheses_depth == 0:
                    start_index = index + 1
        elif character == "}":
            raise RuntimeError(f"Mismatched comment braces: {line}")
        elif character == "{":
            inside_brace = True
            if parentheses_depth == 0:
                game_text.append(line[start_index:index])
        elif character == "(":
            if parentheses_depth == 0:
                game_text.append(line[start_index:index])
            parentheses_depth += 1
        elif character == ")":
            if parentheses_depth == 0:
                raise RuntimeError(f"Mismatched RAV parentheses: {line}")
            parentheses_depth -= 1
            if parentheses_depth == 0:
                start_index = index + 1

    if not inside_brace and parentheses_depth == 0:
        game_text.append(line[start_index:])

    return " ".join(filter(None, map(str.strip, game_text)))


def read_all_games(game_file_name: str) -> Iterator[Game_Record]:
    with open(game_file_name) as input:
        try:
            while True:
                yield Game_Record(input)
        except No_More_Games:
            return


def print_sorted_count_table(count_list: Iterable[tuple[Any, int]]):
    value_column_width = max(len(str(item[0])) for item in count_list)
    count_column_width = len(str(max(item[1] for item in count_list)))
    for value, count in sorted(count_list, key=lambda item: item[1]):
        print(f"{value:{value_column_width}} {count:{count_column_width}}")
