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

unique_ids = unique(data);
offspring_counts = zeros(size(unique_ids));

unique_ids_last_10p = unique(data_last_10p);
offspring_counts_last_10p = zeros(size(unique_ids_last_10p));

for index = 1 : length(unique_ids)
    id = unique_ids(index);
    offspring_counts(index) = sum(id == data);
end
[frequency, child_count] = hist(offspring_counts, max(offspring_counts));
child_count = child_count - adj;
child_count = child_count(frequency > 0);
frequency = frequency(frequency > 0);

for index = 1 : length(unique_ids_last_10p)
    id = unique_ids_last_10p(index);
    offspring_counts_last_10p(index) = sum(id == data_last_10p);
end
[frequency_last_10p, child_count_last_10p] = hist(offspring_counts_last_10p, max(offspring_counts_last_10p));
child_count_last_10p = child_count_last_10p - adj;
child_count_last_10p = child_count_last_10p(frequency_last_10p > 0);
frequency_last_10p = frequency_last_10p(frequency_last_10p > 0);

figure;
hold all;

% Data
plot(child_count, frequency, '.', 'LineWidth', 3, 'DisplayName', 'Data');
plot(child_count_last_10p, frequency_last_10p, '.', 'LineWidth', 3, 'DisplayName', 'Data (last 10%)');

% Null-hypothesis fit
A = sum(child_count.*frequency)/2;
A_last_10p = sum(child_count_last_10p.*frequency_last_10p)/2;
x_fit = linspace(min(child_count), max(child_count), 1000);
x_fit_last_10p = linspace(min(child_count_last_10p), max(child_count_last_10p), 1000);
A_text='';
if min(x_fit) == 0
    A_text = '1/2 ';
end
y_fit = A*(1/2).^(x_fit);
y_fit_last_10p = A_last_10p*(1/2).^(x_fit_last_10p);
plot(x_fit, y_fit, 'LineWidth', 3, 'DisplayName', 'Null hypothesis');
plot(x_fit_last_10p, y_fit_last_10p, 'LineWidth', 3, 'DisplayName', 'Null hypothesis (last 10%)');

% Error bars
high_err = sqrt(frequency);
high_err_last_10p = sqrt(frequency_last_10p);
low_err = min(high_err, frequency - min(y_fit));
low_err_last_10p = min(high_err_last_10p, frequency_last_10p - min(y_fit_last_10p));
errorbar(child_count, frequency, low_err, high_err, '.');
errorbar(child_count_last_10p, frequency_last_10p, low_err_last_10p, high_err_last_10p, '.');

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
set(fit_box, 'position', [xl(2)/10, yl(1)*((yl(2)/yl(1))^(1/5))]);
print([raw_data_file_name '_offspring_plot_log.png']);
close;
