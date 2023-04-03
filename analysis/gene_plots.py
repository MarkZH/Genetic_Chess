#!/usr/bin/python


"""
TODO

1. legends don't appear in special plots.
2. summary special plots don't work at all.

"""

import sys
import numpy as np
import matplotlib.pyplot as plt

gene_pool_filename = sys.argv[1]
filename = gene_pool_filename + '_parsed.txt'

data = np.genfromtxt(filename, delimiter=',', names=True)
id_list = [int(row[0]) for row in data]
column_headers = [name.replace('__', ' - ').replace('_', ' ') for name in data.dtype.names]
xaxis_linewidth = 0.5
xaxis_linecolor = [0.4, 0.4, 0.4]

special_plots = {}

piece_strength_figure, piece_strength_axes = plt.subplots()
piece_strength_prefix = 'Piece Strength Gene'
piece_strength_axes.set_title('Piece Strength Evolution')
piece_count = 0
piece_end_values = {}
special_plots['piece strength'] = (piece_strength_figure, piece_strength_axes)
piece_strength_plots = []
piece_strength_labels = []

opening_priority_figure, opening_priority_axes = plt.subplots()
opening_priority_suffix = ' - Priority - Opening'
opening_priority_axes.set_title('Opening Gene Priority Evolution')
opening_priority_count = 0
special_plots['gene priorities opening'] = (opening_priority_figure, opening_priority_axes)
opening_priority_plots = []
opening_priority_labels = []

endgame_priority_figure, endgame_priority_axes = plt.subplots()
endgame_priority_suffix = ' - Priority - Endgame'
endgame_priority_axes.set_title('Endgame Gene Priority Evolution')
endgame_priority_count = 0
special_plots['gene priorities endgame'] = (endgame_priority_figure, endgame_priority_axes)
endgame_priority_plots = []
endgame_priority_labels = []

first_order_move_figure, first_order_move_axes = plt.subplots()
sort_order_prefix = 'Move Sorting Gene - Sorter Order - '
first_order_move_axes.set_title('Move Sorter Order Preference (1 = Highest Priority)')
special_plots['sorting order preferences'] = (first_order_move_figure, first_order_move_axes)
first_order_plots = []
first_order_labels = []

shorten = {'Castling Possible Gene': 'Castling',
           'Checkmate Material Gene': 'Checkmate',
           'Freedom to Move Gene': 'Freedom',
           'King Confinement Gene': 'Confine',
           'King Protection Gene': 'Protect',
           'Opponent Pieces Targeted Gene': 'Target',
           'Passed Pawn Gene': 'Passed',
           'Pawn Advancement Gene': 'Advance',
           'Sphere of Influence Gene': 'Sphere',
           'Total Force Gene': 'Force',
           'Pawn Structure Gene': sort_order_prefix}

marker_size = 5
line_width = 1

invalid_plot = plt.figure()
plt.close(invalid_plot)


# Plot evolution of individual genes
for yi in range(1, len(column_headers)):
    this_data = np.array([datum[yi] for datum in data])
    name = column_headers[yi]
    is_sorter_count = name == 'Move Sorting Gene - Sorter Count'
    if is_sorter_count:
        max_count = int(max(this_data))
        split_data = np.zeros((len(this_data), max_count + 1), int)
        game_count = np.array(range(1, len(this_data) + 1)).T
        sorter_count_ymax = 0
        for count in range(max_count + 1):
            percents = 100*np.cumsum(this_data == count)/game_count
            split_data[:, count] = percents
            ymax = max(percents[int(np.ceil(0.01*len(percents))):])
            sorter_count_ymax = max(ymax, sorter_count_ymax)
        this_data = split_data

    is_sorter_order = name in sort_order_prefix
    if not is_sorter_order:
        this_figure, these_axes = plt.subplots()

        for column in range(1):
            p = these_axes.plot(id_list, this_data, '.', markersize=marker_size)
            if is_sorter_count:
                these_axes.text(id_list[-1]*1.02, this_data[-1, column], str(column - 1), color=p[0].get_color())

        these_axes.set_xlabel(column_headers[0])
        if is_sorter_count:
            these_axes.set_ylabel('Percent of games')
            these_axes.set_ylim(0, sorter_count_ymax)
        these_axes.axhline(color='k', linewidth=0.5)

        these_axes.set_title(name)

        this_figure.savefig(gene_pool_filename + ' gene ' + name + '.png')
        plt.close(this_figure)

    plot_figure = invalid_plot
    if name in piece_strength_prefix:
        plot_figure, plot_axes = piece_strength_figure, piece_strength_axes
    elif name in opening_priority_suffix:
        plot_figure, plot_axes = opening_priority_figure, opening_priority_axes
    elif name in endgame_priority_suffix:
        plot_figure, plot_axes = endgame_priority_figure, endgame_priority_axes
    elif is_sorter_order:
        plot_figure, plot_axes = first_order_move_figure, first_order_move_axes

    if plot_figure != invalid_plot:
        if is_sorter_order:
            conv_window = 10000
            smooth_data = np.conv(this_data, np.ones(conv_window, 1), 'valid')/conv_window
        else:
            conv_window = 100
            smooth_data = np.movmean(this_data, conv_window, 'endpoints', 'discard')

        conv_margin = np.floor(conv_window/2)
        x_axis = id_list[conv_margin - 1 : -conv_margin]

        p = plot_axes.plot(x_axis, smooth_data, linewidth=line_width)

        if plot_figure == piece_strength_figure:
            name = name[-1]
            piece_count = piece_count + 1
            make_dashed = (piece_count > 7)
            piece_end_values[name] = str(smooth_data[-1], '%.2f')
            piece_strength_plots.append(p)
            piece_strength_labels.append(name + ' (' + piece_end_values[name] + ')')
        elif plot_figure == opening_priority_figure:
            opening_priority_plots.append(p)
            opening_priority_labels.append(shorten[name[-len(opening_priority_suffix)]])
            opening_priority_count = opening_priority_count + 1
            make_dashed = (opening_priority_count > 7)
        elif plot_figure == endgame_priority_figure:
            endgame_priority_plots.append(p)
            endgame_priority_labels.append(shorten[name[-len(endgame_priority_suffix)]])
            endgame_priority_count = endgame_priority_count + 1
            make_dashed = (endgame_priority_count > 7)
        elif plot_figure == first_order_move_figure:
            first_order_plots.append(p)
            first_order_labels.append(name[len(sort_order_prefix) - 1:])

        if make_dashed:
            set(p, 'LineStyle', ':')

print('# Piece values')
for piece in piece_end_values.keys():
    print(piece + ' = ' + piece_end_values[piece])

print('# Priority Plot Key')
for gene in shorten.keys():
    print(shorten[gene] + ' --> ' + gene)

# Create special summary plots
for name in special_plots.keys():
    special_plot, special_axes = special_plots[name]
    special_axes.axhline(color='k', linewidth=0.2)

    if special_plot == piece_strength_figure:
        leg = special_plot.legend(piece_strength_plots, piece_strength_labels, loc='outside right', alignment='left')
    elif special_plot == opening_priority_figure:
        leg = special_plot.legend(opening_priority_plots, opening_priority_labels)
    elif special_plot == endgame_priority_figure:
        leg = special_plot.legend(endgame_priority_plots, endgame_priority_labels)
    elif special_plot == first_order_move_figure:
        leg = special_plot.legend(first_order_plots, first_order_labels)
    
    special_axes.set_xlabel('ID')

    special_plot.savefig(gene_pool_filename + ' special ' + name + '.png')
    plt.close(special_plot)
