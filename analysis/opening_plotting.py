#!/usr/bin/python

from collections import Counter, defaultdict
import numpy as np
import matplotlib.pyplot as plt
import common


def get_openings(game_file_name: str) -> list[list[str]]:
    opening_moves = []
    white_opening_moves = []
    black_opening_moves = []
    unique_opening_counter: dict[str, int] = defaultdict(int)
    for game in common.read_all_games(game_file_name):
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

    def all_first_moves(color):
        pawn_ranks = (3, 4) if color == "white" else (6, 5)
        return ([f"{file}{rank}" for file in "abcdefgh" for rank in pawn_ranks]
                + [f"N{file}{pawn_ranks[0]}" for file in "acfh"])

    all_white_first_moves = all_first_moves("white")
    all_black_first_moves = all_first_moves("black")
    all_first_plies = [f"{white_move} {black_move}"
                       for white_move in all_white_first_moves
                       for black_move in all_black_first_moves]

    print("\n# Most popular openings:")
    common.print_sorted_count_table(unique_opening_counter.items())
    print(f"\n{len(unique_opening_counter)} unique openings played.")
    print(f"Not played: {', '.join(sorted(set(all_first_plies) - set(unique_opening_counter)))}")

    return [opening_moves, white_opening_moves, black_opening_moves]


def opening_cumulative_percent_usage(openings):
    open_count = Counter(openings)
    top = sorted(open_count.keys(), key=open_count.get)[-20:]
    game_counts = np.arange(len(openings)) + 1
    other = "Other"
    opening_stats = {other: 0}
    opening_stats.update({opening: 100*np.cumsum([x == opening for x in openings])/game_counts for opening in top})
    if len(open_count) > 20:
        opening_stats[other] = 100 - sum(opening_stats.values())
    else:
        del opening_stats[other]
    return opening_stats


def plot_opening(openings: list[str], plot_title: str, game_file_name: str):
    top_openings = opening_cumulative_percent_usage(openings)
    game_counts = np.arange(len(openings)) + 1
    labels = list(top_openings.keys())

    figure, axes = plt.subplots()
    stacks = axes.stackplot(game_counts, *top_openings.values(), labels=labels)
    axes.set_xlabel('Games played')
    axes.set_ylabel('Cumulative percent of games')
    legend_handles, legend_labels = axes.get_legend_handles_labels()
    for label, stack in zip(legend_labels, stacks):
        if label == "Other":
            stack.set_facecolor("gray")
            stack.set_hatch("//")
    plot_label_count = 20
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


def plot_all_openings(game_file: str):
    plot_titles = ["First move counts", "White's first move counts", "Black's first move counts"]
    for openings, plot_title in zip(get_openings(game_file), plot_titles):
        plot_opening(openings, plot_title, game_file)
