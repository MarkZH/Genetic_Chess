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

% Data
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
xlabel(data.colheaders(1));
ylabel(data.colheaders(2));
title('Offspring Count');

print([raw_data_file_name '_offspring_plot_lin.png']);
set(gca, 'yscale', 'log');
xl = xlim;
yl = ylim;
set(fit_box, 'position', [xl(2)/10, yl(1)*((yl(2)/yl(1))^(1/5))]);
print([raw_data_file_name '_offspring_plot_log.png']);
close;
