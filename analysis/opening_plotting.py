#!/usr/bin/python

import sys
import os
from collections import Counter, defaultdict
from typing import Dict, List
import numpy as np
import matplotlib.pyplot as plt
from delete_comments import delete_comments

picture_file_args = {'dpi': 600, 'format': 'png'}
pic_ext = picture_file_args['format']


def get_opening_files(game_file: str) -> List[str]:
    outfile = f"{game_file}_opening_list.txt"
    white_file = outfile + "_white.txt"
    black_file = outfile + "_black.txt"
    unique_opening_counter: Dict[str, int] = defaultdict(lambda: 0)
    with open(game_file) as input, open(outfile, 'w') as output, open(white_file, 'w') as white_output, open(black_file, 'w') as black_output:
        for line in input:
            if not line.startswith('1. '):
                continue

            line = delete_comments(line)
            parts = line.split()

            def get_part(index):
                return parts[index] if len(parts) > index and not parts[index][0].isdigit() else ""
            
            white_move = get_part(1)
            black_move = get_part(2)
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
    os.remove(parsed_file_name)

    figure, axes = plt.subplots()
    game_counts = np.array(range(1, top_data.size + 1))
    ymax = 0
    for col in range(len(top_data[0])):
        opening_counts = np.cumsum(np.array([row[col] for row in top_data]))
        percents = 100*(opening_counts/game_counts)
        axes.plot(game_counts, percents, linewidth=2, label=top_data.dtype.names[col].replace("_", " "))
        max_percent = max(percents[int(np.ceil(0.01*len(percents))) : -1])
        ymax = max(ymax, max_percent)

    axes.set_xlabel('Games played')
    axes.set_ylabel('Percent of games')
    axes.set_ylim(0, ymax)
    axes.legend()
    axes.set_title(plot_title)
    figure.savefig(f'{file_name}_opening_moves_plot.{pic_ext}', **picture_file_args)
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


if __name__ == "__main__":
    plot_all_openings(sys.argv[1])