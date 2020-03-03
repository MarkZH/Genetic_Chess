warning('off', 'Octave:axis-non-positive-log-limits');

function freq_0 = frequency_plot(data, reproduction_adjustment, data_label, plot_handle)
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
    y_fit = A*(1/2).^(x_fit);
    plot(x_fit, y_fit, 'LineWidth', 3, 'DisplayName', ['Null hypothesis (' data_label ')']);

    % Error bars
    high_err = sqrt(frequency);
    low_err = high_err;
    fix_range = (low_err < 1.4); % sqrt(2) with margin
    low_err(fix_range) = 0.95*low_err(fix_range);
    errorbar(child_count, frequency, low_err, high_err, '.');

    freq_0 = A;
end

function add_labels_and_save(raw_data_file_name, data_labels, A_text, f0s, plot_handle)
    text_boxes = [];

    figure(plot_handle);
    text_boxes = [];
    set(gca, 'yscale', 'linear');
    xl = xlim;
    yl = ylim;
    xlim([0 xl(2)]);
    xl = xlim;
    for idx = 1 : length(data_labels)
        A = f0s(idx);
        data_label = data_labels{idx};

        % Info box
        fit_text = {['y = A(1/2)^{x} (' data_label ')'],
                    ['A = ' num2str(A) ' = ' A_text ' # games = ' A_text ' # offspring'],
                    'Error bars show \pm{}2 standard deviations'};

        fit_box_lin_position = [0.5*xl(2), (0.7 - idx/7)*yl(2)];
        fit_box = text(fit_box_lin_position(1), fit_box_lin_position(2), fit_text);
        text_boxes(end + 1) = fit_box;
    end
    print([raw_data_file_name '_offspring_plot_lin.png']);

    set(gca, 'yscale', 'log');
    xl = xlim;
    yl = ylim;
    xlim([0 xl(2)]);
    xl = xlim;
    for idx = 1 : length(data_labels)
        A = f0s(idx);
        data_label = data_labels{idx};

        fit_box_log_position = [xl(2)/15, yl(1)*((yl(2)/yl(1))^((2-idx/2)/5))];
        set(text_boxes(idx), 'position', fit_box_log_position);
    end
    print([raw_data_file_name '_offspring_plot_log.png']);
end

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
    graphics_toolkit("gnuplot");
    args = argv();
    if length(args) > 0
        filename = args{1};
        if strcmp(args{2}, "cloning")
            adj=1;
        else
            adj=0;
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

if adj == 1
    A_text = '1/2 ';
else
    A_text='';
end

f = figure;
data_labels = {'All', 'Last 10%'};
f0all = frequency_plot(data, adj, data_labels{1}, f);
f010p = frequency_plot(data_last_10p, adj, data_labels{2}, f);
add_labels_and_save(raw_data_file_name, data_labels, A_text, [f0all, f010p], f);
close all;
