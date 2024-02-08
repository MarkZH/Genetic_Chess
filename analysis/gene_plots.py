#!/usr/bin/python

import numpy as np
import numpy.typing as npt
import matplotlib.pyplot as plt
from typing import Any, Type
import common


def add_value_to_data_line(data_line: list, header_line: list[str], title: str, value: Any, data_type: Type) -> None:
    index = header_line.index(title)
    if data_line[index]:
        raise Exception('Value already found: ' + title + ' for ID ' + str(data_line[0]))
    data_line[index] = data_type(value)


def parse_gene_pool(gene_pool_file_name: str) -> tuple[list[str], npt.NDArray]:
    # Read gene file for gene names
    header_line: list[str] = []
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
    parsed_data = []
    with open(gene_pool_file_name) as f:
        new_data_line: list[int | float | None] = [None]*len(header_line)
        data_line = new_data_line.copy()
        current_gene = ''
        for line in f:
            line = line.split('#')[0].strip()
            if not line:
                continue

            if line == 'END':
                data_line = [x or 0 for x in data_line]
                parsed_data.append(data_line)
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
                    data_type: Type
                    if current_gene:
                        if parameter == "Sorter Order":
                            sorters = [name.strip() for name in value.split(',')]
                            for sorter in sorters:
                                title = current_gene + ' - ' + parameter + ' - ' + sorter
                                value = str(sorters.index(sorter) + 1)
                                add_value_to_data_line(data_line, header_line, title, value, int)
                            continue
                        else:
                            title = current_gene + ' - ' + parameter
                            data_type = float
                    else:
                        title = parameter  # ID
                        data_type = int

                    add_value_to_data_line(data_line, header_line, title, value, data_type)

    return header_line, np.array(parsed_data)


def plot_genome(gene_pool_filename: str) -> None:
    column_names, data = parse_gene_pool(gene_pool_filename)
    id_list = data[:, 0]

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

    # Plot evolution of individual genes
    for column_index, column_name in enumerate(column_names[1:], 1):
        this_data = data[:, column_index]
        if column_name.endswith("Activation Begin"):
            activation_begin_data = this_data.copy()
            continue
        name = column_name.replace('__', ' - ').replace('_', ' ')
        is_sorter_count = name == 'Move Sorting Gene - Sorter Count'
        if is_sorter_count:
            max_count = int(max(this_data))
            window = 10000
            split_data = np.zeros((len(this_data) - window + 1, max_count + 1))
            for count in range(max_count + 1):
                ais_with_count = (this_data == count).astype(int)
                split_data[:, count] = 100*common.moving_mean(ais_with_count, window)
            this_data = split_data

        is_sorter_order = name.startswith(sort_order_prefix)
        if not is_sorter_order:
            this_figure, these_axes = plt.subplots()

            for column in range(np.size(this_data, 1) if is_sorter_count else 1):
                label = str(column) if is_sorter_count else None
                is_activation_end = column_name.endswith("Activation End")
                if not label and is_activation_end:
                    label = "End"
                style = '-' if is_sorter_count else '.'
                linewidth = common.plot_params['plot line weight'] if is_sorter_count else None
                markersize = None if is_sorter_count else common.plot_params["scatter dot size"]
                d = this_data[:, column] if is_sorter_count else this_data
                if is_activation_end:
                    these_axes.plot(common.centered_x_axis(id_list, activation_begin_data), activation_begin_data, style, markersize=markersize, linewidth=linewidth, label="Begin")
                these_axes.plot(common.centered_x_axis(id_list, d), d, style, markersize=markersize, linewidth=linewidth, label=label)
                if is_activation_end:
                    these_axes.axhline(y=1.0, color=common.plot_params["x-axis color"], linewidth=common.plot_params["x-axis weight"])
                    legend = these_axes.legend()
                    for line in legend.get_lines():
                        line.set_markersize(line.get_markersize()*5)
                    name = " ".join(name.split()[:-1])

            these_axes.set_xlabel(column_names[0])
            if is_sorter_count:
                these_axes.set_ylabel('Percent of genomes')
                leg = these_axes.legend(fontsize=common.plot_params['legend text size'], bbox_to_anchor=(1.01, 0.5), loc="center left")
                for line in leg.get_lines():
                    line.set_linewidth(2*line.get_linewidth())

            if 'Speculation' not in name:
                these_axes.axhline(color=common.plot_params["x-axis color"], linewidth=common.plot_params["x-axis weight"])

            these_axes.set_title(name)

            pic_ext = common.picture_file_args["format"]
            this_figure.savefig(f'{gene_pool_filename} gene {name}.{pic_ext}', **common.picture_file_args, bbox_inches="tight" if is_sorter_count else None)
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

        conv_window = 10000 if is_sorter_order else 100
        smooth_data = common.moving_mean(this_data, conv_window)
        x_axis = common.centered_x_axis(id_list, smooth_data)

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

        plot_axes.plot(x_axis, smooth_data, linewidth=common.plot_params['plot line weight'], label=label)

    print('# Piece values')
    for piece, value in piece_end_values.items():
        print(f'{piece} = {value}')

    print('\n# Priority Plot Key')
    for gene, short_name in shorten.items():
        print(f'{short_name} --> {gene}')

    # Create special summary plots
    for name, (special_figure, special_axes) in special_plots.items():
        special_axes.axhline(color=common.plot_params["x-axis color"], linewidth=common.plot_params["x-axis weight"])
        leg = special_axes.legend(fontsize=common.plot_params["legend text size"])
        if special_axes == first_order_move_axes:
            for line in leg.get_lines():
                line.set_linewidth(2*line.get_linewidth())
        special_axes.set_xlabel('ID')

        special_figure.savefig(f'{gene_pool_filename} special {name}.{pic_ext}', **common.picture_file_args)
        plt.close(special_figure)
