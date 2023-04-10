#!/usr/bin/python

import sys
import os
import numpy as np
import matplotlib.pyplot as plt


def write_line(file, *args):
    file.write('\t'.join(str(x) for x in args) + '\n')


def parse_game_file(file_name):
    game = 0
    white_wins = 0
    black_wins = 0
    draws = 0
    number_of_moves = 0
    time = 0
    white_time_left = 0
    black_time_left = 0
    plot_data_file_name = file_name + '_plots.txt'
    with open(file_name) as f, open(plot_data_file_name, 'w') as w:
        write_line(w, 'Game', 'White Wins', 'Black Wins', 'Draws', 'Time', 'Result Type', 'White Time Left', 'Black Time Left', 'Number of Moves')
        # result_type key:
        #   0 = Checkmate (white win)
        #   1 = Checkmate (black win)
        #   2 = 50-move
        #   3 = 3-fold repitition
        #   4 = Time forfeit (white win)
        #   5 = Time forfeit (black win)
        #   6 = Insufficient material
        #   7 = No legal moves
        #   8 = Time expired with insufficient material
        for line in f:
            if line.startswith('[Result'):
                game += 1
                result = line.split('"')[1]
                if result == '1/2-1/2':
                    draws += 1
                elif result == '1-0':
                    white_wins += 1
                    result_type = 0
                else:
                    black_wins += 1
                    result_type = 1
            elif line.startswith('[Termination'):
                result_text = line.split('"')[1]
                if result_text.lower() == 'threefold repetition':
                    result_type = 3
                elif result_text.lower() == '50-move limit':
                    result_type = 2
                elif result_text.lower() == 'time forfeiture':
                    result_type += 4
                elif result_text.lower() == 'insufficient material':
                    result_type = 6
                elif result_text.lower() == 'stalemate':
                    result_type = 7
                elif result_text.lower() == 'time expired with insufficient material':
                    result_type = 8
                else:
                    raise Exception('Unrecognized result type: ' + result_text)
            elif line and line[0].isdigit():
                number_of_moves = line.split('. ')[0]
            elif line.startswith('[TimeControl'):
                time = line.split('"')[1]
            elif line.startswith('[TimeLeftWhite'):
                white_time_left = line.split('"')[1]
            elif line.startswith('[TimeLeftBlack'):
                black_time_left = line.split('"')[1]
            elif line.startswith('[Event') and game > 0:
                write_line(w, game, white_wins, black_wins, draws, time, result_type, white_time_left, black_time_left, number_of_moves)
                number_of_moves = 0

        if game > 0:
            write_line(w, game, white_wins, black_wins, draws, time, result_type, white_time_left, black_time_left, number_of_moves)
    return plot_data_file_name


def plot_endgames(file_name, common_plot_params, picture_file_args):
    parsed_data_file_name = parse_game_file(file_name)
    data = np.genfromtxt(parsed_data_file_name, delimiter='\t', names=True)
    os.remove(parsed_data_file_name)
    column_headers = [name.replace('_', ' ') for name in data.dtype.names]

    game_number     = np.array([int(row[0]) for row in data])
    white_wins      = np.array([int(row[1]) for row in data])
    black_wins      = np.array([int(row[2]) for row in data])
    draws           = np.array([int(row[3]) for row in data])
    game_time       = np.array([row[4] for row in data])
    result_type     = np.array([int(row[5]) for row in data])
    white_time_left = np.array([row[6] for row in data])
    black_time_left = np.array([row[7] for row in data])
    moves_in_game   = np.array([int(row[8]) for row in data])

    line_width = common_plot_params["plot line weight"]
    bar_line_width = 0.5
    bar_color = 'k'
    marker_size = common_plot_params["scatter dot size"]
    stat_text_size = 7

    winner_figure, winner_axes = plt.subplots()
    winner_axes.semilogx(game_number, 100*white_wins/game_number, linewidth=line_width, label=f"{column_headers[1]} ({white_wins[-1]})")
    winner_axes.semilogx(game_number, 100*black_wins/game_number, linewidth=line_width, label=f"{column_headers[2]} ({black_wins[-1]})")
    winner_axes.semilogx(game_number, 100*draws/game_number, linewidth=line_width, label=f"{column_headers[3]} ({draws[-1]})")
    winner_axes.set_xlabel(column_headers[0])
    winner_axes.set_ylabel('Percentage')
    winner_axes.legend(fontsize=common_plot_params["legend text size"])
    winner_axes.set_title('Winning Sides')

    pic_ext = picture_file_args["format"]
    winner_figure.savefig(f"{file_name}_game_outcomes.{pic_ext}", **picture_file_args)
    plt.close(winner_figure)

    white_checkmates = result_type == 0
    black_checkmates = result_type == 1
    fifty_moves = result_type == 2
    threefold = result_type == 3
    white_time_win = result_type == 4
    black_time_win = result_type == 5
    material = result_type == 6
    no_legal = result_type == 7
    time_and_material = result_type == 8
    number_of_games = len(game_number)
    if np.logical_or(result_type > 8, result_type < 0).any():
        print('Unknown result types found.')

    outcome_figure, outcome_axes = plt.subplots()
    outcome_axes.loglog(game_number, 100*np.cumsum(white_checkmates)/game_number, nonpositive='mask', linewidth=line_width, label=f'White checkmate ({sum(white_checkmates)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(black_checkmates)/game_number, nonpositive='mask', linewidth=line_width, label=f'Black checkmate ({sum(black_checkmates)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(white_time_win)/game_number, nonpositive='mask', linewidth=line_width, label=f'White wins on time ({sum(white_time_win)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(black_time_win)/game_number, nonpositive='mask', linewidth=line_width, label=f'Black wins on time ({sum(black_time_win)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(fifty_moves)/game_number, nonpositive='mask', linewidth=line_width, label=f'50-move ({sum(fifty_moves)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(threefold)/game_number, nonpositive='mask', linewidth=line_width, label=f'3-fold ({sum(threefold)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(material)/game_number, nonpositive='mask', linewidth=line_width, label=f'Insufficient material ({sum(material)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(no_legal)/game_number, nonpositive='mask', linewidth=line_width, label=f'Stalemate ({sum(no_legal)})')
    outcome_axes.loglog(game_number, 100*np.cumsum(time_and_material)/game_number, nonpositive='mask', linewidth=line_width, label=f'Time expires w/o material ({sum(time_and_material)})')
    outcome_axes.set_xlabel('Games played')
    outcome_axes.set_ylabel('Percentage')
    outcome_axes.legend(fontsize=common_plot_params["legend text size"])
    outcome_axes.set_title('Type of Endgame')

    outcome_figure.savefig(f"{file_name}_game_result_type_frequencies.{pic_ext}", **picture_file_args)
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
        game_time_axes.legend(fontsize=common_plot_params["legend text size"])
        game_time_axes.set_ylim(max_time_left*(-0.10), max_time_left*1.05)
        game_time_axes.set_xlabel('Game number')
        game_time_axes.set_ylabel('Time (sec)')
        game_time_axes.set_title('Time left on clock at end of game')

        game_time_figure.savefig(f'{file_name}_game_time_left.{pic_ext}', **picture_file_args)
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

    move_count_figure.savefig(f'{file_name}_moves_in_game.{pic_ext}', **picture_file_args)
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

    move_count_histogram_axes.legend(fontsize=common_plot_params["legend text size"])
    move_count_histogram_figure.savefig(f'{file_name}_moves_in_game_histogram.{pic_ext}', **picture_file_args)
    plt.close(move_count_histogram_figure)

    winning_games_lengths = moves_in_game[white_checkmates | black_checkmates]
    winning_move_counts, winning_move_bins = np.histogram(winning_games_lengths, range(1, max(moves_in_game) + 1))
    checkmate_figure, checkmate_axes = plt.subplots()
    checkmate_axes.bar(winning_move_bins[0:-1], winning_move_counts, width=1, facecolor=bar_color, edgecolor=bar_color, linewidth=bar_line_width, label='All checkmates')
    checkmate_axes.set_title('Checkmate game lengths')
    checkmate_axes.set_xlim(0, max_game_length_display)

    # Log-normal fit
    bins_fit = winning_move_bins[winning_move_bins > 0]
    mean_log = np.mean(np.log(winning_games_lengths))
    std_log = np.std(np.log(winning_games_lengths))
    winning_games_count = len(winning_games_lengths)
    fit = winning_games_count*np.exp(-.5*np.power((np.log(bins_fit) - mean_log)/std_log, 2))/(bins_fit*std_log*np.sqrt(2*np.pi))
    checkmate_axes.plot(bins_fit, fit, linewidth=line_width, label='Log-normal fit')

    checkmate_axes.set_xlabel('Moves in Game')
    checkmate_axes.set_ylabel(f'Counts (total = {winning_games_count})')

    stats = [f'Mean = {np.mean(winning_games_lengths):.2f}',
             f'Median = {np.median(winning_games_lengths):.2f}',
             f'Std. Dev. = {np.std(winning_games_lengths):.2f}',
             f'Min = {min(winning_games_lengths)}',
             f'Max = {max(winning_games_lengths)}',
             '',
             f'Log-Norm Peak = {np.exp(mean_log - np.power(std_log, 2)):.2f}',
             f'Log-Norm Width = {std_log:.2f}']

    xl = checkmate_axes.get_xlim()
    yl = checkmate_axes.get_ylim()
    checkmate_axes.text(0.65*xl[1], 0.5*yl[1], '\n'.join(stats), fontsize=stat_text_size)

    checkmate_axes.legend(fontsize=common_plot_params["legend text size"])
    checkmate_figure.savefig(f'{file_name}_moves_in_game_histogram_checkmate.{pic_ext}', **picture_file_args)
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
    other_endgame_axes.legend(fontsize=common_plot_params["legend text size"])

    other_endgame_axes.set_xlim([0, max_game_length_display])
    other_endgame_figure.savefig(f'{file_name}_moves_in_game_histogram_draw.{pic_ext}', **picture_file_args)
    plt.close(other_endgame_figure)

    timeout_games = (white_time_win | black_time_win | time_and_material)
    timeout_counts, timeout_bins = np.histogram(moves_in_game[timeout_games], range(1, max(moves_in_game) + 1))
    timeout_figure, timeout_axes = plt.subplots()
    timeout_axes.bar(timeout_bins[0:-1], timeout_counts, width=1, facecolor=bar_color, edgecolor=bar_color, linewidth=bar_line_width)
    timeout_axes.set_title('Timeout game lengths')
    timeout_axes.set_xlabel('Moves in Game')
    timeout_axes.set_ylabel(f'Counts (total = {sum(timeout_games)})')
    timeout_axes.set_xlim(0, max_game_length_display)

    timeout_figure.savefig(f'{file_name}_moves_in_game_histogram_timeout.{pic_ext}', **picture_file_args)
    plt.close(timeout_figure)


if __name__ == "__main__":
    plot_endgames(sys.argv[1])
