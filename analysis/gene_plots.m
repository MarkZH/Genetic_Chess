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

priority_figure = figure;
priority_suffix = ' Gene - Priority';
priority_count = 0;
title('Gene Priority Evolution');

speculation_keyword = 'Speculation';
speculation_figure = figure;
title('Speculation Constants');

ancestor_prefix = 'Ancestors';
original_pool_prefix = 'Original';

special_plots = [0, 0, 0];
file_name_suffixes = {'_piece_strength.png', '_gene_priorities.png', '_speculation.png'};

for yi = 2 : length(data.colheaders) - 2
    this_data = data.data(:, yi);
    name_list = data.colheaders(yi);
    name = name_list{1};
    if(all(int64(this_data) == this_data)) % integer data
        % Offset integer data to see all pools
        this_data = this_data + 0.1*(pool_ids - max(pool_ids)/2);
    end

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

    plot(xlim, [0 0], 'k'); % X-axis

    for index = 1:length(id_marks)
        plot(id_marks(index)*[1 1], ylim, 'displayname', id_notes{index});
    end

    leg = legend('show');
    set(leg, 'location', 'southoutside');
    set(leg, 'orientation', 'horizontal');
    legend left;

    conv_window = 100;
    smooth_data = conv(this_data, ones(conv_window, 1), 'valid')/conv_window;
    conv_margin = floor(conv_window/2);
    x_axis = id_list(conv_margin : end - conv_margin);
    if isempty(findstr(name, ancestor_prefix)) && isempty(findstr(name, original_pool_prefix))
        plot(x_axis, smooth_data, 'k', 'LineWidth', 3, 'displayname', 'Average');
    else
        if isempty(findstr(name, original_pool_prefix))
            ylabel('Fraction of ancestry');
        end
    end

    print([gene_pool_filename '_gene_' name '.png']);

    special_plot_index = 0;
    if ~isempty(findstr(name, piece_strength_prefix)) && isempty(findstr(name, original_pool_prefix))
        plot_figure = piece_strength_figure;
        special_plot_index = 1;
    elseif ~isempty(findstr(name, priority_suffix))
        plot_figure = priority_figure;
        special_plot_index = 2;
    elseif ~isempty(findstr(name, speculation_keyword))
        plot_figure = speculation_figure;
        special_plot_index = 3;
    end

    if special_plot_index > 0 && length(this_data) > conv_window;
        figure(plot_figure);
        hold all;

        make_dashed = false;
        if special_plot_index == 1
            name = name(end);
        elseif special_plot_index == 2
            name = name(1 : end - length(priority_suffix));
            priority_count = priority_count + 1;
            make_dashed = (priority_count > 7);
        elseif special_plot_index == 3
            cutoff = findstr('-', name);
            name = name(cutoff + 2 : end);
        end

        p = plot(x_axis, smooth_data, 'LineWidth', 3, 'displayname', name);
        if make_dashed
            set(p, 'LineStyle', ':');
        end
        special_plots(special_plot_index) = plot_figure;
    end
end

for index = 1 : length(special_plots)
    if ~special_plots(index)
        continue;
    end

    figure(special_plots(index));

    plot(xlim, [0 0], 'k'); % X-axis

    if special_plots(index) == speculation_figure
        plot(xlim, [1 1], 'k'); % default speculation constant
    end

    for id_index = 1:length(id_marks)
        plot(id_marks(id_index)*[1 1], ylim, 'displayname', id_notes{id_index});
    end

    leg = legend('show');
    set(leg, 'orientation', 'horizontal');
    set(leg, 'location', 'southoutside');
    legend left;

    xlabel('ID');

    print([gene_pool_filename file_name_suffixes{index}]);
end
