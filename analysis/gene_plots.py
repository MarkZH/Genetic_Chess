#!/usr/bin/python

import sys
import os
import numpy as np
import matplotlib.pyplot as plt


def add_value_to_data_line(data_line, header_line, title, value):
    index = header_line.index(title)
    if data_line[index]:
        raise Exception('Value already found: ' + title + ' for ID ' + str(data_line[0]))
    data_line[index] = value


def parse_gene_pool(gene_pool_file_name):
    # Read gene file for gene names
    header_line = []
    current_gene = ''
    with open(gene_pool_file_name) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            if ':' in line:
                parameter, value = line.split(':', 1)
                parameter = parameter.strip()
                if parameter == 'Still Alive':
                    continue
                elif parameter == 'ID':
                    header_line.append(parameter)
                elif parameter == 'Name':
                    current_gene = value.strip()
                elif parameter == 'Sorter Order':
                    sorters = [name.strip() for name in value.split(',')]
                    for sorter in sorters:
                        header_line.append(current_gene + " - " + parameter + " - " + sorter)
                else:
                    header_line.append(current_gene + ' - ' + parameter)
            elif line == 'END':
                break
            else:
                raise Exception('Unknown line format: ' + line)

    # Read gene pool file for data
    output_file_name = gene_pool_file_name + '_parsed.txt'
    with open(gene_pool_file_name) as f, open(output_file_name, 'w') as w:
        new_data_line = ['']*len(header_line)
        data_line = new_data_line.copy()
        current_gene = ''
        w.write(','.join(header_line) + '\n')
        for line in f:
            line = line.split('#')[0].strip()
            if not line:
                continue

            if line == 'END':
                data_line = [x or '0' for x in data_line]
                w.write(','.join(data_line) + '\n')
                current_gene = ''
                data_line = new_data_line.copy()
            elif ':' in line:
                parameter, value = line.split(':', 1)
                value = value.strip()
                if parameter == 'Name':
                    current_gene = value
                elif parameter == 'Still Alive':
                    continue
                else:
                    if current_gene:
                        if parameter == "Sorter Order":
                            sorters = [name.strip() for name in value.split(',')]
                            for sorter in sorters:
                                title = current_gene + ' - ' + parameter + ' - ' + sorter
                                value = str(sorters.index(sorter) + 1)
                                add_value_to_data_line(data_line, header_line, title, value)
                            continue
                        else:
                            title = current_gene + ' - ' + parameter
                    else:
                        title = parameter  # ID

                    add_value_to_data_line(data_line, header_line, title, value)

    return output_file_name


def plot_genome(gene_pool_filename: str) -> str:
    parsed_data_file_name = parse_gene_pool(gene_pool_filename)
    picture_file_args = {'dpi': 600, 'format': 'png'}
    pic_ext = picture_file_args['format']

    data = np.genfromtxt(parsed_data_file_name, delimiter=',', names=True)
    os.remove(parsed_data_file_name)
    id_list = [int(row[0]) for row in data]
    column_headers = [name.replace('__', ' - ').replace('_', ' ') for name in data.dtype.names]

    special_plots = {}

    piece_strength_figure, piece_strength_axes = plt.subplots()
    piece_strength_prefix = 'Piece Strength Gene'
    piece_strength_axes.set_title('Piece Strength Evolution')
    piece_end_values = {}
    special_plots['piece strength'] = (piece_strength_figure, piece_strength_axes)

    opening_priority_figure, opening_priority_axes = plt.subplots()
    opening_priority_suffix = ' - Priority - Opening'
    opening_priority_axes.set_title('Opening Gene Priority Evolution')
    special_plots['gene priorities opening'] = (opening_priority_figure, opening_priority_axes)

    endgame_priority_figure, endgame_priority_axes = plt.subplots()
    endgame_priority_suffix = ' - Priority - Endgame'
    endgame_priority_axes.set_title('Endgame Gene Priority Evolution')
    special_plots['gene priorities endgame'] = (endgame_priority_figure, endgame_priority_axes)

    first_order_move_figure, first_order_move_axes = plt.subplots()
    sort_order_prefix = 'Move Sorting Gene - Sorter Order - '
    first_order_move_axes.set_title('Move Sorter Order Preference (1 = Highest Priority)')
    special_plots['sorting order preferences'] = (first_order_move_figure, first_order_move_axes)

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
               'Pawn Structure Gene': 'Structure'}

    marker_size = 1
    line_width = 1

    # Plot evolution of individual genes
    for yi, name in enumerate(column_headers[1:], 1):
        this_data = np.array([datum[yi] for datum in data])
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

        is_sorter_order = name.startswith(sort_order_prefix)
        if not is_sorter_order:
            this_figure, these_axes = plt.subplots()

            for column in range(1 if this_data.ndim == 1 else np.size(this_data, 1)):
                p = these_axes.plot(id_list, this_data, '.', markersize=marker_size)
                if is_sorter_count:
                    these_axes.text(id_list[-1]*1.02, this_data[-1, column], str(column), color=p[0].get_color())

            these_axes.set_xlabel(column_headers[0])
            if is_sorter_count:
                these_axes.set_ylabel('Percent of games')
                these_axes.set_ylim(0, sorter_count_ymax)

            if 'Speculation' not in name:
                these_axes.axhline(color='k', linewidth=0.5)

            these_axes.set_title(name)

            this_figure.savefig(f'{gene_pool_filename} gene {name}.{pic_ext}', **picture_file_args)
            plt.close(this_figure)

        if name.startswith(piece_strength_prefix):
            plot_axes = piece_strength_axes
        elif name.endswith(opening_priority_suffix):
            plot_axes = opening_priority_axes
        elif name.endswith(endgame_priority_suffix):
            plot_axes = endgame_priority_axes
        elif is_sorter_order:
            plot_axes = first_order_move_axes
        else:
            continue

        conv_window = 1000 if is_sorter_order else 100
        smooth_data = np.convolve(this_data, np.ones(conv_window), mode="valid")/conv_window
        conv_margin = int(np.floor(conv_window/2))
        x_axis = id_list[conv_margin - 1 : -conv_margin]

        if plot_axes == piece_strength_axes:
            piece_symbol = name[-1]
            end_value = f'{smooth_data[-1]:.2f}'
            piece_end_values[piece_symbol] = end_value
            label = f'{piece_symbol} ({end_value})'
        elif plot_axes == opening_priority_axes:
            label = shorten[name[:-len(opening_priority_suffix)]]
        elif plot_axes == endgame_priority_axes:
            label = shorten[name[:-len(endgame_priority_suffix)]]
        elif plot_axes == first_order_move_axes:
            label = name[len(sort_order_prefix) - 1:]

        plot_axes.plot(x_axis, smooth_data, linewidth=line_width, label=label)

    print('# Piece values')
    for piece, value in piece_end_values.items():
        print(f'{piece} = {value}')

    print('\n# Priority Plot Key')
    for gene, short_name in shorten.items():
        print(f'{short_name} --> {gene}')

    # Create special summary plots
    for name, (special_figure, special_axes) in special_plots.items():
        special_axes.axhline(color='k', linewidth=0.2)
        special_axes.legend()
        special_axes.set_xlabel('ID')

        special_figure.savefig(f'{gene_pool_filename} special {name}.{pic_ext}', **picture_file_args)
        plt.close(special_figure)


if __name__ == "__main__":
    plot_genome(sys.argv[1])
