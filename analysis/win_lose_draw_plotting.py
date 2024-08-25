#!/usr/bin/python

from enum import Enum, auto
import numpy as np
import matplotlib.pyplot as plt
import common


class Game_Ending(Enum):
    WHITE_MATE = auto()
    BLACK_MATE = auto()
    FIFTY_MOVE = auto()
    THREEFOLD = auto()
    WHITE_TIME_WIN = auto()
    BLACK_TIME_WIN = auto()
    MATERIAL_DRAW = auto()
    STALEMATE = auto()
    TIME_WITHOUT_MATERIAL = auto()
    WHITE_RESIGNATION = auto()
    BLACK_RESIGNATION = auto()


def extract_game_endings(game_file_name: str):
    game_count = 0
    data = []
    column_names = ['Game', 'White Wins', 'Black Wins', 'Draws', 'Time', 'Result Type', 'White Time Left', 'Black Time Left', 'Number of Moves']
    for game in common.read_all_games(game_file_name):
        result = game.headers["Result"]
        game_count += 1
        white_wins = 0
        black_wins = 0
        draws = 0
        if result == '1/2-1/2':
            draws = 1
        elif result == '1-0':
            white_wins = 1
            result_type = Game_Ending.WHITE_MATE
        else:
            black_wins = 1
            result_type = Game_Ending.BLACK_MATE

        try:
            result_text = game.headers["GameEnding"]
            if result_text.lower() == 'threefold repetition':
                result_type = Game_Ending.THREEFOLD
            elif result_text.lower() == '50-move limit':
                result_type = Game_Ending.FIFTY_MOVE
            elif result_text.lower() == 'time forfeiture':
                result_type = Game_Ending.WHITE_TIME_WIN if result_type == Game_Ending.WHITE_MATE else Game_Ending.BLACK_TIME_WIN
            elif result_text.lower() == 'insufficient material':
                result_type = Game_Ending.MATERIAL_DRAW
            elif result_text.lower() == 'stalemate':
                result_type = Game_Ending.STALEMATE
            elif result_text.lower() == 'time expired with insufficient material':
                result_type = Game_Ending.TIME_WITHOUT_MATERIAL
            elif result_text.lower() ==  "white resigned":
                result_type = Game_Ending.WHITE_RESIGNATION
            elif result_text.lower() ==  "black resigned":
                result_type = Game_Ending.BLACK_RESIGNATION
            else:
                raise Exception('Unrecognized result type: ' + result_text)
        except KeyError:
            pass

        time = float(game.headers["TimeControl"])
        white_time_left = float(game.headers["TimeLeftWhite"])
        black_time_left = float(game.headers["TimeLeftBlack"])
        number_of_moves = (len(game.moves) + 1)//2
        data.append([game_count, white_wins, black_wins, draws, time, result_type, white_time_left, black_time_left, number_of_moves])

    return column_names, data


def plot_endgames(file_name: str):
    names, data = extract_game_endings(file_name)

    def get_data(name: str, number_type: type) -> tuple[np.ndarray, str]:
        index = names.index(name)
        return np.array([number_type(row[index]) for row in data]), name

    game_number, game_number_label = get_data("Game", int)
    white_wins, white_wins_label = get_data("White Wins", int)
    black_wins, black_wins_label = get_data("Black Wins", int)
    draws, draws_label = get_data("Draws", int)
    game_time, _ = get_data("Time", np.float64)
    result_type, _ = get_data("Result Type", Game_Ending)
    white_time_left, _ = get_data("White Time Left", np.float64)
    black_time_left, _ = get_data("Black Time Left", np.float64)
    moves_in_game, _ = get_data("Number of Moves", int)

    line_width = common.plot_params["plot line weight"]
    bar_line_width = 0.5
    bar_color = common.plot_params["bar color"]
    marker_size = common.plot_params["scatter dot size"]
    stat_text_size = 7

    winner_figure, winner_axes = plt.subplots()

    def draw_result_plot(result_data, label):
        win_lose_draw_window = 1000
        result_percentage = 100*common.moving_mean(result_data, win_lose_draw_window)
        total = np.sum(result_data)
        x_axis = common.centered_x_axis(game_number, result_percentage)
        winner_axes.plot(x_axis, result_percentage, linewidth=line_width, label=f"{label} ({total})")

    draw_result_plot(white_wins, white_wins_label)
    draw_result_plot(black_wins, black_wins_label)
    draw_result_plot(draws, draws_label)

    winner_axes.set_xlabel(game_number_label)
    winner_axes.set_ylabel('Percentage')
    winner_axes.legend(fontsize=common.plot_params["legend text size"])
    winner_axes.set_title('Winning Sides')

    pic_ext = common.picture_file_args["format"]
    winner_figure.savefig(f"{file_name}_game_outcomes.{pic_ext}", **common.picture_file_args)
    plt.close(winner_figure)

    white_checkmates = result_type == Game_Ending.WHITE_MATE
    black_checkmates = result_type == Game_Ending.BLACK_MATE
    fifty_moves = result_type == Game_Ending.FIFTY_MOVE
    threefold = result_type == Game_Ending.THREEFOLD
    white_time_win = result_type == Game_Ending.WHITE_TIME_WIN
    black_time_win = result_type == Game_Ending.BLACK_TIME_WIN
    material = result_type == Game_Ending.MATERIAL_DRAW
    no_legal = result_type == Game_Ending.STALEMATE
    time_and_material = result_type == Game_Ending.TIME_WITHOUT_MATERIAL
    white_resignations = result_type == Game_Ending.WHITE_RESIGNATION
    black_resignations = result_type == Game_Ending.BLACK_RESIGNATION
    number_of_games = len(game_number)

    outcome_figure, outcome_axes = plt.subplots()

    def draw_outcome_plot(outcome, label):
        outcome_window = 10000
        outcome_percentage = 100*common.moving_mean(outcome, outcome_window)
        x_axis = common.centered_x_axis(game_number, outcome_percentage)
        outcome_axes.semilogy(x_axis, outcome_percentage, linewidth=line_width, label=label)

    draw_outcome_plot(white_checkmates, "White checkmate")
    draw_outcome_plot(black_checkmates, "Black checkmate")
    draw_outcome_plot(white_time_win, "White time win")
    draw_outcome_plot(black_time_win, "Black time win")
    draw_outcome_plot(fifty_moves, "50-move")
    draw_outcome_plot(threefold, "3-fold")
    draw_outcome_plot(material, "Material draw")
    draw_outcome_plot(no_legal, "Stalemate")
    draw_outcome_plot(time_and_material, "Time w/o material")
    draw_outcome_plot(white_resignations, "White resigned")
    draw_outcome_plot(black_resignations, "Black resigned")

    outcome_axes.set_xlabel('Games played')
    outcome_axes.set_ylabel('Percentage')
    outcome_axes.set_ylim(top=100)
    outcome_axes.set_title('Type of Endgame')
    leg = outcome_axes.legend(fontsize=common.plot_params["legend text size"], loc=[1.05, 0.3])
    for line in leg.get_lines():
        line.set_linewidth(2*line.get_linewidth())
    outcome_figure.tight_layout()

    outcome_figure.savefig(f"{file_name}_game_result_type_frequencies.{pic_ext}", **common.picture_file_args)
    plt.close(outcome_figure)

    if max(game_time) > 0:
        game_time_figure, game_time_axes = plt.subplots()
        avg_time_left = (white_time_left + black_time_left)/2
        max_time_left = max(max(white_time_left), max(black_time_left))
        below_zero = -0.05*max_time_left
        below_zero_random = below_zero*(1.5 - np.random.rand(*white_time_left.shape))
        white_time_left[white_time_left < 0] = below_zero_random[white_time_left < 0]
        black_time_left[black_time_left < 0] = below_zero_random[black_time_left < 0]

        game_time_axes.plot(game_number, white_time_left, '.k', markersize=marker_size)
        game_time_axes.plot(game_number, black_time_left, '.k', markersize=marker_size)
        window = 100
        convolve_window = np.ones(window)/window
        x_margin = int(np.floor(window/2))
        avg_x_axis = game_number[x_margin - 1 : -x_margin]
        game_time_axes.plot(avg_x_axis,
                            np.convolve(avg_time_left, convolve_window, mode="valid"),
                            'r',
                            linewidth=line_width,
                            label='Moving average')
        game_time_axes.legend(fontsize=common.plot_params["legend text size"])
        game_time_axes.set_ylim(max_time_left*(-0.10), max_time_left*1.05)
        game_time_axes.set_xlabel('Game number')
        game_time_axes.set_ylabel('Time (sec)')
        game_time_axes.set_title('Time left on clock at end of game')

        game_time_figure.savefig(f'{file_name}_game_time_left.{pic_ext}', **common.picture_file_args)
        plt.close(game_time_figure)

    # Don't plot top 0.1% of longest games to make trends easier to see
    max_game_count = np.floor(0.999*len(moves_in_game))
    move_counts, move_bins = np.histogram(moves_in_game, range(1, max(moves_in_game) + 1))
    total_counts = 0
    max_game_length_display = move_bins[-1]
    for index, count in enumerate(move_counts):
        total_counts += count
        if total_counts > max_game_count:
            max_game_length_display = move_bins[index]
            break

    move_count_figure, move_count_axes = plt.subplots()
    move_count_axes.plot(game_number, moves_in_game, '.k', markersize=marker_size)
    move_count_axes.set_xlabel('Game number')
    move_count_axes.set_ylabel('Moves in Game')
    move_count_axes.set_title('Number of moves in game')
    move_count_axes.set_ylim(0, max_game_length_display)

    move_count_figure.savefig(f'{file_name}_moves_in_game.{pic_ext}', **common.picture_file_args)
    plt.close(move_count_figure)

    move_count_histogram_figure, move_count_histogram_axes = plt.subplots()
    move_count_histogram_axes.bar(move_bins[0:-1], move_counts, width=1, facecolor=bar_color, edgecolor=bar_color, linewidth=bar_line_width, label='All game lengths')
    move_count_histogram_axes.set_xlabel('Moves in Game')
    move_count_histogram_axes.set_ylabel(f'Counts (total = {number_of_games})')
    move_count_histogram_axes.set_title('Number of moves in game')
    move_count_histogram_axes.set_xlim([0, max_game_length_display])
    mean_moves = np.mean(moves_in_game)
    std_dev = np.std(moves_in_game)

    # Log-normal fit
    moves_in_game_fit = moves_in_game[moves_in_game > 0]
    bins_fit = move_bins[move_bins > 0]
    mean_log = np.mean(np.log(moves_in_game_fit))
    std_log = np.std(np.log(moves_in_game_fit))
    fit = number_of_games*np.exp(-.5*np.power((np.log(bins_fit) - mean_log)/std_log, 2))/(bins_fit*std_log*np.sqrt(2*np.pi))
    move_count_histogram_axes.plot(bins_fit, fit, linewidth=line_width, label='Log-Normal fit')

    stats = [f'Mean = {mean_moves:.2f}',
             f'Median = {np.median(moves_in_game):.2f}',
             f'Std. Dev. = {std_dev:.2f}',
             f'Min = {min(moves_in_game)}',
             f'Max = {max(moves_in_game)}',
             '',
             f'Log-Norm Peak = {np.exp(mean_log - np.power(std_log, 2)):.2f}',
             f'Log-Norm Width = {std_log:.2f}']

    xl = move_count_histogram_axes.get_xlim()
    yl = move_count_histogram_axes.get_ylim()
    move_count_histogram_axes.text(0.65*xl[1], 0.5*yl[1], '\n'.join(stats), fontsize=stat_text_size)

    move_count_histogram_axes.legend(fontsize=common.plot_params["legend text size"])
    move_count_histogram_figure.savefig(f'{file_name}_moves_in_game_histogram.{pic_ext}', **common.picture_file_args)
    plt.close(move_count_histogram_figure)

    winning_games_lengths = moves_in_game[white_checkmates | black_checkmates]
    winning_move_counts, winning_move_bins = np.histogram(winning_games_lengths, range(max(moves_in_game) + 1))
    checkmate_figure, checkmate_axes = plt.subplots()
    checkmate_axes.bar(winning_move_bins[0:-1], winning_move_counts, width=1, facecolor=bar_color, edgecolor=bar_color, linewidth=bar_line_width, label='All checkmates')
    checkmate_axes.set_title('Checkmate game lengths')
    checkmate_axes.set_xlim(0, max_game_length_display)

    def log_normal_fit(game_lengths, game_length_bins, axes):
        game_lengths = game_lengths[game_lengths > 1]
        bins_fit = game_length_bins[game_length_bins > 0]
        mean_log = np.mean(np.log(game_lengths))
        std_log = np.std(np.log(game_lengths))
        games_count = len(game_lengths)
        fit = games_count*np.exp(-.5*np.power((np.log(bins_fit) - mean_log)/std_log, 2))/(bins_fit*std_log*np.sqrt(2*np.pi))
        axes.plot(bins_fit, fit, linewidth=line_width, label='Log-normal fit')

        axes.set_xlabel('Moves in Game')
        axes.set_ylabel(f'Counts (total = {games_count})')

        stats = [f'Mean = {np.mean(game_lengths):.2f}',
                f'Median = {np.median(game_lengths):.2f}',
                f'Std. Dev. = {np.std(game_lengths):.2f}',
                f'Min = {min(game_lengths)}',
                f'Max = {max(game_lengths)}',
                '',
                f'Log-Norm Peak = {np.exp(mean_log - np.power(std_log, 2)):.2f}',
                f'Log-Norm Width = {std_log:.2f}']

        xl = axes.get_xlim()
        yl = axes.get_ylim()
        axes.text(0.65*xl[1], 0.5*yl[1], '\n'.join(stats), fontsize=stat_text_size)

    log_normal_fit(winning_games_lengths, winning_move_bins, checkmate_axes)
    checkmate_axes.legend(fontsize=common.plot_params["legend text size"])
    checkmate_figure.savefig(f'{file_name}_moves_in_game_histogram_checkmate.{pic_ext}', **common.picture_file_args)
    plt.close(checkmate_figure)

    drawn_games = (fifty_moves | threefold | material | no_legal)
    drawn_counts, drawn_bins = np.histogram(moves_in_game[drawn_games], range(1, max(moves_in_game) + 1))
    fifty_counts, fifty_bins = np.histogram(moves_in_game[fifty_moves], range(1, max(moves_in_game) + 1))
    threefold_counts, threefold_bins = np.histogram(moves_in_game[threefold], range(1, max(moves_in_game) + 1))
    material_counts, material_bins = np.histogram(moves_in_game[material], range(1, max(moves_in_game) + 1))
    no_legal_counts, no_legal_bins = np.histogram(moves_in_game[no_legal], range(1, max(moves_in_game) + 1))

    other_endgame_figure, other_endgame_axes = plt.subplots()
    other_endgame_axes.bar(drawn_bins[0:-1], drawn_counts, width=1, facecolor=bar_color, edgecolor=bar_color, linewidth=bar_line_width, label='All draws')
    other_endgame_axes.plot(fifty_bins[0:-1], fifty_counts, linewidth=line_width, label='Fifty moves')
    other_endgame_axes.plot(threefold_bins[0:-1], threefold_counts, linewidth=line_width, label='3-fold')
    other_endgame_axes.plot(material_bins[0:-1], material_counts, linewidth=line_width, label='Material')
    other_endgame_axes.plot(no_legal_bins[0:-1], no_legal_counts, linewidth=line_width, label='Stalemate')
    other_endgame_axes.set_title('Draw game lengths')
    other_endgame_axes.set_xlabel('Moves in Game')
    other_endgame_axes.set_ylabel(f'Counts (total = {sum(drawn_games)})')
    other_endgame_axes.legend(fontsize=common.plot_params["legend text size"])

    other_endgame_axes.set_xlim([0, max_game_length_display])
    other_endgame_figure.savefig(f'{file_name}_moves_in_game_histogram_draw.{pic_ext}', **common.picture_file_args)
    plt.close(other_endgame_figure)

    timeout_games = (white_time_win | black_time_win | time_and_material)
    timeout_counts, timeout_bins = np.histogram(moves_in_game[timeout_games], range(1, max(moves_in_game) + 1))
    timeout_figure, timeout_axes = plt.subplots()
    timeout_axes.bar(timeout_bins[0:-1], timeout_counts, width=1, facecolor=bar_color, edgecolor=bar_color, linewidth=bar_line_width)
    timeout_axes.set_title('Timeout game lengths')
    timeout_axes.set_xlabel('Moves in Game')
    timeout_axes.set_ylabel(f'Counts (total = {sum(timeout_games)})')
    timeout_axes.set_xlim(0, max_game_length_display)

    timeout_figure.savefig(f'{file_name}_moves_in_game_histogram_timeout.{pic_ext}', **common.picture_file_args)
    plt.close(timeout_figure)

    resignation_games = (white_resignations | black_resignations)
    resignation_game_lengths = moves_in_game[resignation_games]
    resignation_counts, resignation_bins = np.histogram(resignation_game_lengths, range(max(moves_in_game) + 1))
    resignation_figure, resignation_axes = plt.subplots()
    resignation_axes.bar(resignation_bins[0:-1], resignation_counts, width=1, facecolor=bar_color, edgecolor=bar_color, linewidth=bar_line_width)
    resignation_axes.set_title("Resignation game lengths")
    resignation_axes.set_xlabel("Moves in Game")
    resignation_axes.set_ylabel(f"Counts (total = {sum(resignation_games)})")
    resignation_axes.set_xlim(0, max_game_length_display)
    log_normal_fit(resignation_game_lengths, resignation_bins, resignation_axes)

    resignation_figure.savefig(f"{file_name}_moves_in_game_histogram_resignation.{pic_ext}", **common.picture_file_args)
