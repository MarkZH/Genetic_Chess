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


def parse_opening_list(openings):
    open_count = Counter(openings)
    top20 = reversed(sorted(open_count.keys(), key=open_count.get)[-20:])
    return {opening: np.array([x == opening for x in openings], dtype=float) for opening in top20}


def plot_opening(openings: list[str], plot_title: str, game_file_name: str):
    top_openings = parse_opening_list(openings)
    figure, axes = plt.subplots()
    for opening_name, opening_usage in top_openings.items():
        opening_frequency = common.moving_mean(opening_usage, 10000)
        game_counts = np.arange(len(opening_usage)) + 1
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
    figure.savefig(f'''{game_file_name}_opening_moves_plot_{plot_title.split()[0].split("'")[0].lower()}.{common.picture_file_args["format"]}''',
                   **common.picture_file_args,
                   bbox_inches="tight")
    plt.close(figure)


def plot_all_openings(all_games: list[common.Game_Record], game_file: str):
    plot_titles = ["First move counts", "White's first move counts", "Black's first move counts"]
    for openings, plot_title in zip(get_openings(all_games), plot_titles):
        plot_opening(openings, plot_title, game_file)
