#!/usr/bin/python

from collections import Counter, defaultdict
import numpy as np
import matplotlib.pyplot as plt
import common


def get_openings(all_games: list[common.Game_Record]) -> list[list[str]]:
    opening_moves = []
    white_opening_moves = []
    black_opening_moves = []
    unique_opening_counter: dict[str, int] = defaultdict(int)
    for game in all_games:
        white_move = game.moves[0].strip() if game.moves else ""
        black_move = game.moves[1].strip() if len(game.moves) > 1 else ""
        opening = f"{white_move} {black_move}".strip()
        if opening:
            opening_moves.append(opening)
            unique_opening_counter[opening] += 1
        if white_move:
            white_opening_moves.append(white_move)
        if black_move:
            black_opening_moves.append(black_move)

    print("\n# Most popular openings:")
    common.print_sorted_count_table(unique_opening_counter.items())

    return [opening_moves, white_opening_moves, black_opening_moves]


def opening_cumulative_percent_usage(openings):
    open_count = Counter(openings)
    top = sorted(open_count.keys(), key=open_count.get)
    game_counts = np.arange(len(openings)) + 1
    return {opening: 100*np.cumsum([x == opening for x in openings])/game_counts for opening in top}


def plot_opening(openings: list[str], plot_title: str, game_file_name: str):
    top_openings = opening_cumulative_percent_usage(openings)
    game_counts = np.arange(len(openings)) + 1
    labels = list(top_openings.keys())
    plot_label_count = 20
    labels[:-plot_label_count] = ["_"]*(len(labels) - plot_label_count)

    figure, axes = plt.subplots()
    axes.stackplot(game_counts, *top_openings.values(), labels=labels)
    axes.set_xlabel('Games played')
    axes.set_ylabel('Cumulative percent of games')
    legend_handles, legend_labels = axes.get_legend_handles_labels()
    leg = axes.legend(reversed(legend_handles), reversed(legend_labels),
                      fontsize=common.plot_params["legend text size"],
                      bbox_to_anchor=(1.01, 0.5), loc="center left",
                      title=f"Top {plot_label_count}" if len(top_openings) > plot_label_count else None)
    for line in leg.get_lines():
        line.set_linewidth(2*line.get_linewidth())

    axes.set_title(plot_title)
    figure.savefig(f'''{game_file_name}_opening_moves_plot_{plot_title.split()[0].split("'")[0].lower()}.{common.picture_file_args["format"]}''',
                   **common.picture_file_args,
                   bbox_inches="tight")
    plt.close(figure)


def plot_all_openings(all_games: list[common.Game_Record], game_file: str):
    plot_titles = ["First move counts", "White's first move counts", "Black's first move counts"]
    for openings, plot_title in zip(get_openings(all_games), plot_titles):
        plot_opening(openings, plot_title, game_file)
