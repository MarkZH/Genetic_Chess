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

    count_column_width = len(str(max(unique_opening_counter.values())))
    totals = [(count, opening) for opening, count in unique_opening_counter.items()]
    print("\n# Most popular openings:")
    for count, opening in sorted(totals):
        spaces = ' '*(count_column_width - len(str(count)))
        print(spaces, count, opening)

    return [opening_moves, white_opening_moves, black_opening_moves]


def parse_opening_list(openings):
    open_count = Counter(openings)
    top20 = list(reversed(sorted(open_count.keys(), key=lambda x: open_count[x])[-20:]))

    markers = []
    for opening in openings:
        markers.append([x == opening for x in top20])

    return np.array(markers), top20


def plot_opening(openings: list[str], plot_title: str, game_file_name: str):
    top_data, names = parse_opening_list(openings)

    figure, axes = plt.subplots()
    game_counts = np.array(range(1, top_data.size + 1))
    for index, opening_name in enumerate(names):
        opening_frequency = common.moving_mean(top_data[:, index].astype(float), 10000)
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
    figure.savefig(f'''{game_file_name}_{plot_title.split()[0].split("'")[0].lower()}_opening_moves_plot.{common.picture_file_args["format"]}''',
                   **common.picture_file_args,
                   bbox_inches="tight")
    plt.close(figure)


def plot_all_openings(all_games: list[common.Game_Record], game_file: str):
    plot_titles = ["First move counts", "White's first move counts", "Black's first move counts"]
    for openings, plot_title in zip(get_openings(all_games), plot_titles):
        plot_opening(openings, plot_title, game_file)
