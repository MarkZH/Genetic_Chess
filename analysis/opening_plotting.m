warning('off');

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
game_marks_file = 0;
directory = '';
if isOctave
    graphics_toolkit("gnuplot");
    args = argv();
    if length(args) > 0
        filename = args{1};
        if length(args) > 1
            game_marks_file = args{2};
        end
    end
end

if filename == 0
    [filename, directory, ~] = uigetfile();
end

if filename == 0
    return
end
raw_data = fullfile(directory, filename);

if game_marks_file != 0
    data = importdata(fullfile(directory, game_marks_file), ';', 1);
    game_marks = data.data(:,2)';
    game_notes = data.textdata(2:end);
else
    game_marks = [];
end

if isOctave
    python('analysis/opening_plot.py', ['"' raw_data '"']);
end

data = importdata([raw_data, '_opening_plots.txt'], ',');

figure('Position', [0, 0, 1200, 1000]);
hold all;
for col = 1 : size(data.data, 2)
    plot(cumsum(data.data(:, col)),  ...
         'LineWidth', 3, ...
         'displayname', data.colheaders{col});
end

for index = 1:length(game_marks)
    plot(game_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
end

xlabel('Games played');
ylabel('Total Count');
leg = legend('show');
set(leg, 'location', 'northwest');
title('Count of opening moves');
print([raw_data '_opening_moves_plot_lin.png']);
