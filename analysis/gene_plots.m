isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
    args = argv();
    if length(args) > 0
        filename = args{1};
    end
end

if filename == 0
    [filename, directory, ~] = uigetfile();
end

if filename == 0
    return
end

gene_pool_filename = fullfile(directory, filename);

if isOctave
    python('analysis/gene_pool_analyze.py', ['"' gene_pool_filename '"']);
end

filename = [gene_pool_filename '_parsed.txt'];

data = importdata(filename, ',');
id_list = data.data(:, 1);

xaxis_list = data.colheaders(1);
xaxis = xaxis_list{1};
xaxis_linewidth = 0.5;
xaxis_linecolor = [0.4, 0.4, 0.4];

special_plots = containers.Map;

piece_strength_figure = figure;
hold all;
piece_strength_prefix = 'Piece Strength Gene';
title('Piece Strength Evolution');
piece_count = 0;
piece_end_values = containers.Map;
special_plots('piece strength') = piece_strength_figure;
piece_strength_plots = [];
piece_strength_labels = {};

opening_priority_figure = figure;
hold all;
opening_priority_suffix = ' - Priority - Opening';
title('Opening Gene Priority Evolution');
opening_priority_count = 0;
special_plots('gene priorities opening') = opening_priority_figure;
opening_priority_plots = [];
opening_priority_labels = {};

endgame_priority_figure = figure;
hold all;
endgame_priority_suffix = ' - Priority - Endgame';
title('Endgame Gene Priority Evolution');
endgame_priority_count = 0;
special_plots('gene priorities endgame') = endgame_priority_figure;
endgame_priority_plots = [];
endgame_priority_labels = {};

first_order_move_figure = figure;
hold all;
first_order_prefix = 'Move Sorting Gene - Sorter Order - ';
title('First Move Sorter Preference');
special_plots('first order preference') = first_order_move_figure;
first_order_plots = [];
first_order_labels = {};

shorten = containers.Map;
shorten('Castling Possible Gene') = 'Castling';
shorten('Checkmate Material Gene') = 'Checkmate';
shorten('Freedom to Move Gene') = 'Freedom';
shorten('King Confinement Gene') = 'Confine';
shorten('King Protection Gene') = 'Protect';
shorten('Opponent Pieces Targeted Gene') = 'Target';
shorten('Passed Pawn Gene') = 'Passed';
shorten('Pawn Advancement Gene') = 'Advance';
shorten('Sphere of Influence Gene') = 'Sphere';
shorten('Total Force Gene') = 'Force';
shorten('Pawn Structure Gene') = 'Structure';

marker_size = 5;
line_width = 1;

invalid_plot = figure;
close(invalid_plot);

% Plot evolution of individual genes
for yi = 2 : length(data.colheaders)
    this_data = data.data(:, yi);
    name_list = data.colheaders(yi);
    name = name_list{1};
    is_sorter_count = strcmp(name, 'Move Sorting Gene - Sorter Count');
    if is_sorter_count
        max_count = max(this_data);
        split_data = zeros(length(this_data), max_count + 1);
        for count = 0 : max_count
            split_data(:, count + 1) = cumsum(this_data == count);
        end
        this_data = split_data;
    end

    if isempty(strfind(name, first_order_prefix))
        figure;
        hold all;
        for column = 1 : size(this_data, 2)
            plot(id_list, this_data(:, column), ...
                '.', ...
                'markersize', marker_size);
            if is_sorter_count
                text(ceil(id_list(end)*1.02), this_data(end, column), num2str(column - 1));
            end
        end

        xlabel(xaxis);
        plot(xlim, [0 0], 'color', xaxis_linecolor, 'linewidth', xaxis_linewidth); % X-axis

        title(name);

        print([gene_pool_filename ' gene ' name '.png']);
        close;
    end

    plot_figure = invalid_plot;
    if ~isempty(strfind(name, piece_strength_prefix))
        plot_figure = piece_strength_figure;
    elseif ~isempty(strfind(name, opening_priority_suffix))
        plot_figure = opening_priority_figure;
    elseif ~isempty(strfind(name, endgame_priority_suffix))
        plot_figure = endgame_priority_figure;
    elseif ~isempty(strfind(name, first_order_prefix))
        plot_figure = first_order_move_figure;
    end

    if plot_figure != invalid_plot
        conv_window = 100;
        smooth_data = movmean(this_data, conv_window, 'endpoints', 'discard');
        figure(plot_figure);
        conv_margin = floor(conv_window/2);
        x_axis = id_list(conv_margin : end - conv_margin);
        p = plot(x_axis, smooth_data, 'LineWidth', line_width);

        if plot_figure == piece_strength_figure
            name = name(end);
            piece_count = piece_count + 1;
            make_dashed = (piece_count > 7);
            piece_end_values(name) = num2str(smooth_data(end), '%.2f');
            piece_strength_plots(end + 1) = p;
            piece_strength_labels{end + 1} = [name ' (' piece_end_values(name) ')'];
        elseif plot_figure == opening_priority_figure
            opening_priority_plots(end + 1) = p;
            opening_priority_labels{end + 1} = shorten(name(1 : end - length(opening_priority_suffix)));
            opening_priority_count = opening_priority_count + 1;
            make_dashed = (opening_priority_count > 7);
        elseif plot_figure == endgame_priority_figure
            endgame_priority_plots(end + 1) = p;
            endgame_priority_labels{end + 1} = shorten(name(1 : end - length(endgame_priority_suffix)));
            endgame_priority_count = endgame_priority_count + 1;
            make_dashed = (endgame_priority_count > 7);
        elseif plot_figure == first_order_move_figure
            first_order_plots(end + 1) = p;
            first_order_labels{end + 1} = name(length(first_order_prefix):end);
        end

        if make_dashed
            set(p, 'LineStyle', ':');
        end
    end
end

disp('# Piece values');
for piece = piece_end_values.keys()
    disp([piece{1} ' = ' piece_end_values(piece{1})]);
end

disp('# Priority Plot Key');
for gene = shorten.keys()
    disp([shorten(gene{1}) ' --> ' gene{1}]);
end

% Create special summary plots
for name = special_plots.keys()
    name = name{1};
    special_plot = special_plots(name);
    figure(special_plot);
    plot(xlim, [0 0], 'color', xaxis_linecolor, 'linewidth', xaxis_linewidth); % X-axis

    if special_plot == piece_strength_figure
        leg = legend(piece_strength_plots, piece_strength_labels);
    elseif special_plot == opening_priority_figure
        leg = legend(opening_priority_plots, opening_priority_labels);
    elseif special_plot == endgame_priority_figure
        leg = legend(endgame_priority_plots, endgame_priority_labels);
    elseif special_plot == first_order_move_figure
        leg = legend(first_order_plots, first_order_labels);
    end
    set(leg, 'location', 'eastoutside');
    legend left;

    xlabel('ID');

    print([gene_pool_filename ' special ' name '.png']);
    close;
end
