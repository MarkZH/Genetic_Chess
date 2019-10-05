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

piece_strength_prefix = 'Piece Strength Gene';
piece_strength_figure = figure;
title('Piece Strength Evolution');
max_piece_score = -inf;

priority_figure = figure;
priority_suffix = ' Gene - Priority';
priority_count = 0;
title('Gene Priority Evolution');

speculation_keyword = 'Speculation';
speculation_figure = figure;
title('Speculation Constants');

special_plots = [0, 0];

max_priority_value = -inf;
second_max_priority_value = -inf;
min_priority_value = inf;

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

    for index = 1:length(id_marks)
        plot(id_marks(index)*[1 1], ylim, 'displayname', id_notes{index});
    end

    leg = legend('show');
    set(leg, 'location', 'southoutside');
    set(leg, 'orientation', 'horizontal');
    legend left;

    conv_window = 100;
    smooth_data = movmean(this_data, conv_window, 'endpoints', 'discard');
    conv_margin = floor(conv_window/2);
    x_axis = id_list(conv_margin : end - conv_margin);
    plot(x_axis, smooth_data, 'k', 'LineWidth', 3, 'displayname', 'Average');

    special_plot_index = 0;
    if ~isempty(strfind(name, piece_strength_prefix))
        plot_figure = piece_strength_figure;
        special_plot_index = 1;
    elseif ~isempty(strfind(name, priority_suffix))
        plot_figure = priority_figure;
        special_plot_index = 2;
    end

    plot(xlim, [0 0], 'k'); % X-axis

    print([gene_pool_filename ' gene ' name '.png']);

    if special_plot_index > 0 && length(this_data) > conv_window;
        figure(plot_figure);
        hold all;

        make_dashed = false;
        if special_plot_index == 1
            name = name(end);
            max_piece_score = max(abs(smooth_data(end)), max_piece_score);
        elseif special_plot_index == 2
            name = name(1 : end - length(priority_suffix));
            priority_count = priority_count + 1;
            make_dashed = (priority_count > 7);
            smooth_max = max(smooth_data);
            if smooth_max > max_priority_value
                second_max_priority_value = max_priority_value;
                max_priority_value = smooth_max;
            elseif smooth_max > second_max_priority_value
                second_max_priority_value = smooth_max;
            end
            min_priority_value = min(min(smooth_data), min_priority_value);
        elseif special_plot_index == 3
            cutoff = strfind(name, '-');
            name = name(cutoff + 2 : end);
        end

        p = plot(x_axis, smooth_data, 'LineWidth', 3, 'displayname', name);
        if make_dashed
            set(p, 'LineStyle', ':');
        end
        special_plots(special_plot_index) = plot_figure;
    end
end

% Create special summary plots
file_name_suffixes = {'piece strength', 'gene priorities'};
for index = 1 : length(special_plots)
    if ~special_plots(index)
        continue;
    end

    figure(special_plots(index));

    plot(xlim, [0 0], 'k'); % X-axis

    if special_plots(index) == piece_strength_figure
        yl = ylim;
        xl = xlim;
        for level = -10:10
            % width = fraction of full horizontal plot width
            if mod(abs(level),5) == 0
                width = 0.1;
            else
                width = 0.05;
            end
            tick_height = max_piece_score*(level/10);
            if tick_height < yl(1) || tick_height > yl(2)
                continue;
            end
            plot(xl(2)*[1-width 1], tick_height*[1 1], 'k');
        end
    end

    for id_index = 1:length(id_marks)
        plot(id_marks(id_index)*[1 1], ylim, 'displayname', id_notes{id_index});
    end

    leg = legend('show');
    set(leg, 'orientation', 'horizontal');
    set(leg, 'location', 'southoutside');
    legend left;

    xlabel('ID');

    print([gene_pool_filename ' special ' file_name_suffixes{index} '.png']);

    if special_plots(index) == priority_figure
        ylim(1.25*[min(0, min_priority_value) second_max_priority_value]);
        title(['Lower ' get(get(gca, 'title'), 'string')]);
        print([gene_pool_filename ' special ' file_name_suffixes{index} ' low.png']);
    end
end
