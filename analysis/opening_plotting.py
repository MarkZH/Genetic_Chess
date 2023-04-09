#!/usr/bin/python

import sys
import os
import numpy as np
import matplotlib.pyplot as plt
from opening_parse import parse_opening_list
from openings import get_opening_files

picture_file_args = {'dpi': 600, 'format': 'png'}
pic_ext = picture_file_args['format']

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