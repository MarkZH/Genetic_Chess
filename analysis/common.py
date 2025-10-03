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
        self.headers, first_game_line = game_headers(input_stream)
        self.moves = game_moves(input_stream, first_game_line)
        if not self.headers and not self.moves:
            raise No_More_Games()


def game_headers(input_stream: TextIO) -> tuple[dict[str, str], str]:
    headers: dict[str, str] = {}
    for line in input_stream:
        if line.strip().startswith("["):
            name, value = line.strip().strip("[]").split(maxsplit=1)
            headers[name] = value.strip('"')
        elif headers:
            break
    return headers, line


def game_moves(input: TextIO, previous_line: str = "") -> list[str]:
    game_lines: list[str] = []
    for line in itertools.chain([previous_line], input):
        if line.strip():
            game_lines.append(line)
        elif game_lines:
            break
    game_text = delete_comments("".join(game_lines))
    return list(filter(lambda s: "." not in s, game_text.split()))[:-1]


def delete_comments(all_game_text: str) -> str:
    parentheses_depth = 0
    inside_brace = False
    skipping_rest_of_line = False
    game_text: list[str] = []
    start_index = 0
    for index, character in enumerate(all_game_text):
        if inside_brace:
            if character == "}":
                inside_brace = False
                start_index = index + 1
            continue
        elif character == "}":
            raise RuntimeError(f"Mismatched comment braces at character: {all_game_text}\nAfter moves: {" ".join(game_text)}")

        if skipping_rest_of_line:
            if character == "\n":
                skipping_rest_of_line = False
                start_index = index + 1
            continue

        if parentheses_depth > 0:
            if character == ")":
                parentheses_depth -= 1
                if parentheses_depth == 0:
                    start_index = index + 1
                continue
        elif character == ")":
            raise RuntimeError(f"Mismatched RAV parentheses: {all_game_text}\nAfter moves: {" ".join(game_text)}")

        if character == "{":
            inside_brace = True
            append_game_moves(game_text, all_game_text, parentheses_depth, start_index, index)
        elif character == "(":
            append_game_moves(game_text, all_game_text, parentheses_depth, start_index, index)
            parentheses_depth += 1
        elif character == ";":
            skipping_rest_of_line = True
            append_game_moves(game_text, all_game_text, parentheses_depth, start_index, index)

    if not skipping_rest_of_line and not inside_brace and parentheses_depth == 0:
        game_text.append(all_game_text[start_index:])

    return " ".join(filter(None, map(str.strip, game_text)))


def append_game_moves(game_text: list[str], line: str, parentheses_depth: int, start_index: int, index: int) -> None:
    if parentheses_depth == 0:
        game_text.append(line[start_index:index])


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


def x_axis_scaling(x_values: npt.NDArray) -> tuple[float, str]:
    if np.max(x_values) >= 1e4:
        return 1e3, " ($\\times 1000$)"
    else:
        return 1, ""
