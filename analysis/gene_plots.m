isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
    graphics_toolkit('gnuplot'); % Octave only
    args = argv();
    if length(args) > 0
        filename = args{1};
        id_marks = [];
        if length(args) > 1
            marks_file_name = args{2};
            if ~isempty(marks_file_name)
                data = importdata(marks_file_name, ';', 1);
                id_marks = data.data(:,1)';
                id_notes = data.textdata(2:end);
            end
        end
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
still_alive = logical(data.data(:, end));

xaxis_list = data.colheaders(1);
xaxis = xaxis_list{1};

piece_strength_figure = figure;
hold all;
piece_strength_prefix = 'Piece Strength Gene';
title('Piece Strength Evolution');
piece_count = 0;
piece_end_values = containers.Map;

opening_priority_figure = figure;
hold all;
opening_priority_suffix = ' Gene - Priority - Opening';
title('Opening Gene Priority Evolution');
opening_priority_count = 0;

endgame_priority_figure = figure;
hold all;
endgame_priority_suffix = ' Gene - Priority - Endgame';
title('Endgame Gene Priority Evolution');
endgame_priority_count = 0;

special_plots = containers.Map;
special_plots('piece strength') = piece_strength_figure;
special_plots('gene priorities opening') = opening_priority_figure;
special_plots('gene priorities endgame') = endgame_priority_figure;

% Plot evolution of individual genes
for yi = 2 : length(data.colheaders) - 1
    this_data = data.data(:, yi);
    name_list = data.colheaders(yi);
    name = name_list{1};

    figure;
    hold all;
    plot(id_list, this_data, ...
         '.', ...
         'markersize', 10);
    plot(id_list(still_alive), this_data(still_alive), ...
         'ok', ...
         'markersize', 10, ...
         'linewidth', 1, ...
         'displayname', 'Still Alive');
    xlabel(xaxis);
    title(name);


    leg = legend('show');
    set(leg, 'location', 'southoutside');
    set(leg, 'orientation', 'horizontal');
    legend left;

    conv_window = 100;
    smooth_data = movmean(this_data, conv_window, 'endpoints', 'discard');
    conv_margin = floor(conv_window/2);
    x_axis = id_list(conv_margin : end - conv_margin);
    plot(x_axis, smooth_data, 'k', 'LineWidth', 3, 'displayname', 'Average');
    plot(xlim, [0 0], '--k'); % X-axis

    for index = 1:length(id_marks)
        plot(id_marks(index)*[1 1], ylim, 'displayname', id_notes{index});
    end

    print([gene_pool_filename ' gene ' name '.png']);
    close;

    invalid_plot = figure;
    close(invalid_plot);
    plot_figure = invalid_plot;
    if ~isempty(strfind(name, piece_strength_prefix))
        plot_figure = piece_strength_figure;
    elseif ~isempty(strfind(name, opening_priority_suffix))
        plot_figure = opening_priority_figure;
    elseif ~isempty(strfind(name, endgame_priority_suffix))
        plot_figure = endgame_priority_figure;
    end

    if plot_figure != invalid_plot
        figure(plot_figure);
        if plot_figure == piece_strength_figure
            name = name(end);
            piece_count = piece_count + 1;
            make_dashed = (piece_count > 7);
            piece_end_values(name) = num2str(smooth_data(end), '%.2f');
            display_name = [name ' (' piece_end_values(name) ')'];
        elseif plot_figure == opening_priority_figure
            display_name = name(1 : end - length(opening_priority_suffix));
            opening_priority_count = opening_priority_count + 1;
            make_dashed = (opening_priority_count > 7);
        elseif plot_figure == endgame_priority_figure
            display_name = name(1 : end - length(endgame_priority_suffix));
            endgame_priority_count = endgame_priority_count + 1;
            make_dashed = (endgame_priority_count > 7);
        end

        p = plot(x_axis, smooth_data, 'LineWidth', 3, 'displayname', display_name);
        if make_dashed
            set(p, 'LineStyle', ':');
        end
    end
end

disp('# Piece values');
for piece = piece_end_values.keys()
    disp([piece{1} ' = ' piece_end_values(piece{1})]);
end

% Create special summary plots
for name = special_plots.keys()
    name = name{1};
    special_plot = special_plots(name);
    figure(special_plot);
    plot(xlim, [0 0], '--k'); % X-axis

    for id_index = 1:length(id_marks)
        plot(id_marks(id_index)*[1 1], ylim, 'displayname', id_notes{id_index});
    end

    leg = legend('show');
    set(leg, 'orientation', 'horizontal');
    set(leg, 'location', 'southoutside');
    legend left;

    xlabel('ID');

    print([gene_pool_filename ' special ' name '.png']);
    close;
end
