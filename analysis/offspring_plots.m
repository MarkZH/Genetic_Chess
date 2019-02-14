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
x_axis_name = data.colheaders(1);
y_axis_name = data.colheaders(2);

figure;
hold all;
scatter(child_count, frequency, 'LineWidth', 3);
high_err = sqrt(frequency);
low_err = high_err;
low_err(frequency - low_err <= 0) = 0;
errorbar(child_count, frequency, low_err, high_err, '.');
xlabel(x_axis_name);
ylabel(y_axis_name);
title('Offspring Count');

% Exponential fit assuming y = A*exp(B*x)
% Uses a linear least-squares fit on x and log(y) while putting more weight on large y values
% by minimizing D = sum(y.*(log(y) - a - b*x).^2)
%
% Weisstein, Eric W. "Least Squares Fitting--Exponential."
% From MathWorld--A Wolfram Web Resource.
% http://mathworld.wolfram.com/LeastSquaresFittingExponential.html

x = child_count;
y = frequency;

a = (sum((x.^2).*y)*sum(y.*log(y)) - sum(x.*y)*sum(x.*y.*log(y)))/(sum(y)*sum((x.^2).*y) - sum(x.*y)^2);
b = (sum(y)*sum(x.*y.*log(y)) - sum(x.*y)*sum(y.*log(y)))/(sum(y)*sum((x.^2).*y) - sum(x.*y)^2);

B = -b;
A = exp(a);

x_fit = (min(x) : (max(x) - min(x))/1000 : max(x));
y_fit = A*exp(b*x_fit);

plot(x_fit, y_fit, 'LineWidth', 3);

fit_text = {'y = Ae^{-Bx}',
            ['A = ' num2str(A)],
            ['B = ' num2str(B) ' = log(' num2str(exp(B)) ')']};
xl = xlim;
yl = ylim;
text(0.6*xl(2), 0.4*yl(2), fit_text);

print([raw_data_file_name '_offspring_plot.png']);
set(gca, 'yscale', 'log');
print([raw_data_file_name '_offspring_plot_log.png']);
