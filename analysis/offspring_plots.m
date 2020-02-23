global plot_count = 0;

function frequency_plot(data, reproduction_adjustment, raw_data_file_name, plot_handle)
    global plot_count;
    plot_count = plot_count + 1;

    unique_ids = unique(data);
    offspring_counts = zeros(size(unique_ids));

    for index = 1 : length(unique_ids)
        id = unique_ids(index);
        offspring_counts(index) = sum(id == data);
    end
    [frequency, child_count] = hist(offspring_counts, max(offspring_counts));
    child_count = child_count - reproduction_adjustment;
    child_count = child_count(frequency > 0);
    frequency = frequency(frequency > 0);


    % Data
    figure(plot_handle);
    hold all;
    plot(child_count, frequency, '.', 'LineWidth', 3, 'DisplayName', 'Data');

    % Null-hypothesis fit
    A = sum(child_count.*frequency)/2;
    x_fit = linspace(min(child_count), max(child_count), 1000);
    A_text='';
    if min(x_fit) == 0
        A_text = '1/2 ';
    end
    y_fit = A*(1/2).^(x_fit);
    plot(x_fit, y_fit, 'LineWidth', 3, 'DisplayName', 'Null hypothesis');

    % Error bars
    high_err = sqrt(frequency);
    low_err = min(high_err, frequency - min(y_fit));
    errorbar(child_count, frequency, low_err, high_err, '.');

    % Info box
    fit_text = {'y = A(1/2)^{x}',
                ['A = ' num2str(A) ' = ' A_text ' # games = ' A_text ' # offspring'],
                'Error bars show \pm{}2 standard deviations'};
    xl = xlim;
    xlim([0 xl(2)]);
    yl = ylim;
    fit_box = text(0.5*xl(2), 0.4*yl(2), fit_text);

    leg = legend('show');
    xlabel('Number of offspring');
    ylabel('Frequency');
    title('Offspring Count');
    print([raw_data_file_name '_offspring_plot_lin.png']);

    set(gca, 'yscale', 'log');
    xl = xlim;
    yl = ylim;
    set(fit_box, 'position', [xl(2)/10, yl(1)*((yl(2)/yl(1))^(3-plot_count/5))]);
    print([raw_data_file_name '_offspring_plot_log.png']);
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
frequency_plot(data, adj, raw_data_file_name, f);
frequency_plot(data_last_10p, adj, raw_data_file_name, f);
close all;
