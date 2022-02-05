isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
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

xaxis_list = data.colheaders(1);
xaxis = xaxis_list{1};
xaxis_linewidth = 0.5;
xaxis_linecolor = [0.4, 0.4, 0.4];

piece_strength_figure = figure;
hold all;
piece_strength_prefix = 'Piece Strength Gene';
title('Piece Strength Evolution');
piece_count = 0;
piece_end_values = containers.Map;

opening_priority_figure = figure;
hold all;
opening_priority_suffix = ' - Priority - Opening';
title('Opening Gene Priority Evolution');
opening_priority_count = 0;

endgame_priority_figure = figure;
hold all;
endgame_priority_suffix = ' - Priority - Endgame';
title('Endgame Gene Priority Evolution');
endgame_priority_count = 0;

special_plots = containers.Map;
special_plots('piece strength') = piece_strength_figure;
piece_strength_plots = [];
piece_strength_labels = {};
special_plots('gene priorities opening') = opening_priority_figure;
opening_priority_plots = [];
opening_priority_labels = {};
special_plots('gene priorities endgame') = endgame_priority_figure;
endgame_priority_plots = [];
endgame_priority_labels = {};

shorten = containers.Map;
shorten('Castling Possible Gene') = 'Castling';
shorten('Checkmate Material Gene') = 'Checkmate';
shorten('Freedom to Move Gene') = 'Freedom';
shorten('King Confinement Gene') = 'Confine';
shorten('King Protection Gene') = 'Protect';
shorten('Opponent Pieces Targeted Gene') = 'Target';
shorten('Passed Pawn Gene') = 'Passed';
shorten('Pawn Advancement Gene') = 'Advance';
shorten('Pawn Islands Gene') = 'Islands';
shorten('Sphere of Influence Gene') = 'Sphere';
shorten('Stacked Pawns Gene') = 'Stacked';
shorten('Total Force Gene') = 'Force';
shorten('Pawn Structure Gene') = 'Structure';

marker_size = 5;
line_width = 2;

invalid_plot = figure;
close(invalid_plot);

% Plot evolution of individual genes
for yi = 2 : length(data.colheaders)
    this_data = data.data(:, yi);
    name_list = data.colheaders(yi);
    name = name_list{1};

    figure;
    hold all;
    plot(id_list, this_data, ...
         '.', ...
         'markersize', marker_size);
    xlabel(xaxis);
    plot(xlim, [0 0], 'color', xaxis_linecolor, 'linewidth', xaxis_linewidth); % X-axis

    for index = 1:length(id_marks)
        plot(id_marks(index)*[1 1], ylim, 'displayname', id_notes{index});
    end

    title_name = name;
    if strcmp(name, 'Search Strategy Gene - Search Method')
        title_name = [name ' (0 = Minimax, 1 = Iterative Deepening)'];
        conv_window = 100;
        conv_margin = floor(conv_window/2);
        smooth_data = movmean(this_data, conv_window, 'endpoints', 'discard');
        x_axis = id_list(conv_margin : end - conv_margin);
        plot(x_axis, smooth_data, 'LineWidth', line_width);
    endif
    title(title_name);

    print([gene_pool_filename ' gene ' name '.png']);
    close;

    plot_figure = invalid_plot;
    if ~isempty(strfind(name, piece_strength_prefix))
        plot_figure = piece_strength_figure;
    elseif ~isempty(strfind(name, opening_priority_suffix))
        plot_figure = opening_priority_figure;
    elseif ~isempty(strfind(name, endgame_priority_suffix))
        plot_figure = endgame_priority_figure;
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

    for id_index = 1:length(id_marks)
        plot(id_marks(id_index)*[1 1], ylim, 'displayname', id_notes{id_index});
    end

    if special_plot == piece_strength_figure
        leg = legend(piece_strength_plots, piece_strength_labels);
    elseif special_plot == opening_priority_figure
        leg = legend(opening_priority_plots, opening_priority_labels);
    elseif special_plot == endgame_priority_figure
        leg = legend(endgame_priority_plots, endgame_priority_labels);
    endif
    set(leg, 'location', 'eastoutside');
    legend left;

    xlabel('ID');

    print([gene_pool_filename ' special ' name '.png']);
    close;
end
