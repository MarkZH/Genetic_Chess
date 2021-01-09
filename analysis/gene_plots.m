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
still_alive = logical(data.data(:, end - 1));
pool_ids = data.data(:, end);

xaxis_list = data.colheaders(1);
xaxis = xaxis_list{1};

piece_strength_figure = figure;
piece_strength_prefix = 'Piece Strength Gene';
title('Piece Strength Evolution');
piece_count = 0;
piece_end_values = containers.Map;

priority_figure = figure;
priority_suffix = ' Gene - Priority';
title('Gene Priority Evolution');
priority_count = 0;

active_figure = figure;
active_suffix = ' Gene - Active';
title('Total Genome Active');
total_active = 0;

special_plots = [piece_strength_figure, priority_figure, active_figure];
file_name_suffixes = {'piece strength', 'gene priorities', 'genome active'};

% Plot evolution of individual genes
for yi = 2 : length(data.colheaders) - 2
    this_data = data.data(:, yi);
    name_list = data.colheaders(yi);
    name = name_list{1};

    figure;
    hold all;
    for pool_id = 0 : max(pool_ids)
        id_right = (pool_ids == pool_id);
        good = (still_alive & id_right);
        plot(id_list(id_right), this_data(id_right), ...
             '.', ...
             'markersize', 10, ...
             'displayname', ['Pool ' num2str(pool_id)]);
        h = plot(id_list(good), this_data(good), ...
                 'ok', ...
                 'markersize', 10, ...
                 'linewidth', 1);
        if pool_id == max(pool_ids)
            set(h, 'displayname', 'Still Alive');
        end
    end
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

    special_plot_index = 0;
    draw_now = true;
    if ~isempty(strfind(name, piece_strength_prefix)) && isempty(strfind(name, 'Active'))
        plot_figure = piece_strength_figure;
        special_plot_index = 1;
    elseif ~isempty(strfind(name, priority_suffix))
        plot_figure = priority_figure;
        special_plot_index = 2;
    elseif ~isempty(strfind(name, active_suffix))
        plot_figure = active_figure;
        special_plot_index = 3;
        total_active = total_active + smooth_data;
        draw_now = false;
    end

    if special_plot_index > 0 && length(this_data) > conv_window
        figure(plot_figure);
        hold all;

        make_dashed = false;
        display_name = '';
        if special_plot_index == 1
            name = name(end);
            piece_count = piece_count + 1;
            make_dashed = (piece_count > 7);
            piece_end_values(name) = num2str(round(100*smooth_data(end))/100); % Round to 2 decimal places
            display_name = [name ' (' piece_end_values(name) ')'];
        elseif special_plot_index == 2
            name = name(1 : end - length(priority_suffix));
            priority_count = priority_count + 1;
            make_dashed = (priority_count > 7);
            display_name = [name ' (' num2str(smooth_data(end)) ')'];
        end

        if draw_now
            p = plot(x_axis, smooth_data, 'LineWidth', 3, 'displayname', display_name);
            if make_dashed
                set(p, 'LineStyle', ':');
            end
        end
    end
end

% Create special summary plots
for index = 1 : length(special_plots)
    figure(special_plots(index));

    plot(xlim, [0 0], '--k'); % X-axis

    if special_plots(index) == piece_strength_figure
        disp('# Piece values');
        for piece = piece_end_values.keys()
            disp([piece{1} ' = ' piece_end_values(piece{1})]);
        end
    elseif special_plots(index) == active_figure
        plot(x_axis, total_active, 'LineWidth', 3);
        grid on;
    end

    for id_index = 1:length(id_marks)
        plot(id_marks(id_index)*[1 1], ylim, 'displayname', id_notes{id_index});
    end

    if special_plots(index) != active_figure
        leg = legend('show');
        set(leg, 'orientation', 'horizontal');
        set(leg, 'location', 'southoutside');
        legend left;
    end

    xlabel('ID');

    print([gene_pool_filename ' special ' file_name_suffixes{index} '.png']);
    close;
end
