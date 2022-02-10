warning('off');

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
game_marks_file = 0;
directory = '';
if isOctave
    args = argv();
    if length(args) > 0
        filename = args{1};
        plot_title = args{2};
        if length(args) > 2
            game_marks_file = args{3};
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

top_data = importdata([raw_data, '_top_opening_data.txt'], ',');

figure;
hold all;
for col = 1 : size(top_data.data, 2)
    plot(cumsum(top_data.data(:, col)), ...
         'LineWidth', 2, ...
         'displayname', top_data.colheaders{col});
end

for index = 1:length(game_marks)
    plot(game_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
end

xlabel('Games played');
ylabel('Total Count');
leg = legend('show');
set(leg, 'location', 'eastoutside');
title(plot_title);
print([raw_data '_opening_moves_plot.png']);
close;
