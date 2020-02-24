warning('off', 'Octave:axis-non-positive-log-limits');

global plot_count = 0;
global text_box_references = [];
global text_box_lin_positions = [];
global text_box_log_positions = [];
global original_lin_axes = [];

function frequency_plot(data, reproduction_adjustment, raw_data_file_name, data_label, plot_handle)
    global plot_count;
    plot_count = plot_count + 1;

    indices = data - min(data) + 1;
    valid_indices = unique(indices);
    offspring_counts = accumarray(indices, 1) - reproduction_adjustment;
    offspring_counts = offspring_counts(valid_indices);
    [frequency, child_count] = hist(offspring_counts, min(offspring_counts) : max(offspring_counts));
    child_count = child_count(frequency > 0);
    frequency = frequency(frequency > 0);

    figure(plot_handle);
    hold all;
    leg = legend('show');
    xlabel('Number of offspring');
    ylabel('Frequency');
    title('Offspring Count');

    plot(child_count, frequency, '.', 'LineWidth', 3, 'DisplayName', ['Data (' data_label ')']);

    % Null-hypothesis fit
    A = sum(child_count.*frequency)/2;
    x_fit = linspace(min(child_count), max(child_count), 1000);
    A_text='';
    if min(x_fit) == 0
        A_text = '1/2 ';
    end
    y_fit = A*(1/2).^(x_fit);
    plot(x_fit, y_fit, 'LineWidth', 3, 'DisplayName', ['Null hypothesis (' data_label ')']);

    % Error bars
    high_err = sqrt(frequency);
    low_err = min(high_err, frequency - min(y_fit));
    errorbar(child_count, frequency, low_err, high_err, '.');

    % Info box
    fit_text = {['y = A(1/2)^{x} (' data_label ')'],
                ['A = ' num2str(A) ' = ' A_text ' # games = ' A_text ' # offspring'],
                'Error bars show \pm{}2 standard deviations'};

    set(gca, 'yscale', 'linear');
    global original_lin_axes;
    if isempty(original_lin_axes)
        original_lin_axes = [xlim; ylim];
    else
        xlim(original_lin_axes(1, :));
        ylim(original_lin_axes(2, :));
    end
    global text_box_references;
    global text_box_lin_positions;
    for row = 1 : size(text_box_references, 1)
        set(text_box_references(row), 'position', text_box_lin_positions(row, :));
    end
    xl = xlim;
    yl = ylim;
    fit_box_lin_position = [0.5*xl(2), (0.7 - plot_count/7)*yl(2)];
    fit_box = text(fit_box_lin_position(1), fit_box_lin_position(2), fit_text);
    print([raw_data_file_name '_offspring_plot_lin.png']);

    set(gca, 'yscale', 'log');
    global text_box_log_positions;
    for row = 1 : size(text_box_references, 1)
        set(text_box_references(row), 'position', text_box_log_positions(row, :));
    end
    xl = xlim;
    yl = ylim;
    fit_box_log_position = [xl(2)/15, yl(1)*((yl(2)/yl(1))^((2-plot_count/2)/5))];
    set(fit_box, 'position', fit_box_log_position);
    print([raw_data_file_name '_offspring_plot_log.png']);

    text_box_references(end + 1) = fit_box;
    text_box_lin_positions(end + 1, :) = fit_box_lin_position;
    text_box_log_positions(end + 1, :) = fit_box_log_position;
end

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
adj = 0;
if isOctave
    graphics_toolkit("gnuplot");
    args = argv();
    if length(args) > 0
        filename = args{1};
        if strcmp(args{2}, "cloning")
            adj=1
        end
    end
end

if filename == 0
    [filename, directory, ~] = uigetfile();
end

if filename == 0
    return
end
raw_data_file_name = fullfile(directory, filename);

data = importdata(raw_data_file_name);
data_last_10p = data(floor(0.9*length(data)) : end);

f = figure;
frequency_plot(data, adj, raw_data_file_name, 'All', f);
frequency_plot(data_last_10p, adj, raw_data_file_name, 'Last 10%', f);
close all;
