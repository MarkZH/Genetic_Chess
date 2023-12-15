#!/usr/bin/python

import os
from collections import Counter, defaultdict
import numpy as np
import matplotlib.pyplot as plt
import common


def get_opening_files(game_file: str) -> list[str]:
    outfile = f"{game_file}_opening_list.txt"
    white_file = outfile + "_white.txt"
    black_file = outfile + "_black.txt"
    unique_opening_counter: dict[str, int] = defaultdict(lambda: 0)
    with open(game_file) as input, open(outfile, 'w') as output, open(white_file, 'w') as white_output, open(black_file, 'w') as black_output:
        while True:
            moves = common.game_moves(input)
            if not moves:
                break
            white_move = moves[0].strip() if moves else ""
            black_move = moves[1].strip() if len(moves) > 1 else ""
            opening = f"{white_move} {black_move}".strip()
            if opening:
                output.write(f"{opening}\n")
                unique_opening_counter[opening] += 1
            if white_move:
                white_output.write(f"{white_move}\n")
            if black_move:
                black_output.write(f"{black_move}\n")

    count_column_width = len(str(max(unique_opening_counter.values())))
    totals = [(count, opening) for opening, count in unique_opening_counter.items()]
    print("\n# Most popular openings:")
    for count, opening in sorted(totals):
        spaces = ' '*(count_column_width - len(str(count)))
        print(spaces, count, opening)

    return [outfile, white_file, black_file]


def parse_opening_list(filename):
    with open(filename) as f:
        openings = [line.strip() for line in f]

    open_count = Counter(openings)
    top20 = list(reversed(sorted(open_count.keys(), key=lambda x: open_count[x])[-20:]))

    parsed_file_name = filename + '_top_opening_data.txt'
    with open(parsed_file_name, 'w') as out:
        out.write(','.join(top20) + '\n')
        for opening in openings:
            marker = ['1' if x == opening else '0' for x in top20]
            out.write(','.join(marker) + '\n')

    return parsed_file_name


def plot_opening(file_name: str, plot_title: str):
    parsed_file_name = parse_opening_list(file_name)
    top_data = np.genfromtxt(parsed_file_name, delimiter=',', names=True)
    if not top_data.dtype.names:
        raise ValueError(f"No column names found in {parsed_file_name} from {file_name}.")
    os.remove(parsed_file_name)

    figure, axes = plt.subplots()
    game_counts = np.array(range(1, top_data.size + 1))
    for opening_name in top_data.dtype.names:
        opening_frequency = common.moving_mean(top_data[opening_name], 10000)
        game_axis = common.centered_x_axis(game_counts, opening_frequency)
        axes.plot(game_axis, 100*opening_frequency,
                  linewidth=common.plot_params['plot line weight'],
                  label=opening_name.replace("_", " "))

    axes.set_xlabel('Games played')
    axes.set_ylabel('Percent of games')
    leg = axes.legend(fontsize=common.plot_params["legend text size"], bbox_to_anchor=(1.01, 0.5), loc="center left")
    for line in leg.get_lines():
        line.set_linewidth(2*line.get_linewidth())

    axes.set_title(plot_title)
    figure.savefig(f'{file_name}_opening_moves_plot.{common.picture_file_args["format"]}', **common.picture_file_args, bbox_inches="tight")
    plt.close(figure)


def plot_all_openings(game_file):
    for parsed_file in get_opening_files(game_file):
        if parsed_file.endswith("_white.txt"):
            plot_title = "White's first move counts"
        elif parsed_file.endswith("_black.txt"):
            plot_title = "Black's first move counts"
        else:
            plot_title = "First move counts"
        plot_opening(parsed_file, plot_title)
        os.remove(parsed_file)
