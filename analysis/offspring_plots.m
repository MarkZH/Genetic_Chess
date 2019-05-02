isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
    graphics_toolkit("gnuplot");
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
raw_data_file_name = fullfile(directory, filename);

data = importdata(raw_data_file_name, ';');

child_count = data.data(:, 1);
frequency = data.data(:, 2);

figure;
hold all;

plot(child_count, frequency, '.', 'LineWidth', 3, 'DisplayName', 'Data');
high_err = sqrt(frequency);
low_err = high_err;
low_err(frequency - low_err <= 0) = 0;
errorbar(child_count, frequency, low_err, high_err, '.');

A = sum(child_count.*frequency)/2;
x_fit = linspace(1, max(child_count), 1000);
y_fit = A*(1/2).^(x_fit);

plot(x_fit, y_fit, 'LineWidth', 3, 'DisplayName', 'Null hypothesis');

fit_text = {'y = A(1/2)^{x}',
            ['A = ' num2str(A) ' = # games = # offspring']};
xl = xlim;
yl = ylim;
text(0.6*xl(2), 0.4*yl(2), fit_text);

leg = legend('show');
xlabel(data.colheaders(1));
ylabel(data.colheaders(2));
title('Offspring Count');

print([raw_data_file_name '_offspring_plot.png']);
set(gca, 'yscale', 'log');
print([raw_data_file_name '_offspring_plot_log.png']);
