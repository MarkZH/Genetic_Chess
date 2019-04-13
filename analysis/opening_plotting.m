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

top_data = importdata([raw_data, '_top_opening_data.txt'], ',');

figure;
hold all;
for col = 1 : size(top_data.data, 2)
    plot(cumsum(top_data.data(:, col)),  ...
         'LineWidth', 3, ...
         'displayname', top_data.colheaders{col});
end

for index = 1:length(game_marks)
    plot(game_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
end

xlabel('Games played');
ylabel('Total Count');
leg = legend('show');
set(leg, 'location', 'southoutside');
set(leg, 'orientation', 'horizontal');
title('Count of opening moves');
print([raw_data '_opening_moves_plot_lin.png']);



all_data = importdata([raw_data, '_all_opening_data.txt'], ',');

figure;
hold all;
window = 1000;
end_rate_all_data = sum(all_data.data(end-window:end, :));

[~, top_rate_indices] = sort(end_rate_all_data, 'descend');
top10_rate_indices = top_rate_indices(1:10);

for col = top10_rate_indices
    plot(movsum(all_data.data(:, col), window, 'EndPoints', 'discard'), ...
         'LineWidth', 3, ...
         'displayname', all_data.colheaders{col});
end

for index = 1:length(game_marks)
    plot(game_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
end

xlabel('Games played');
ylabel(['Rate of opening usage (count/' num2str(window) ' games)']);
leg = legend('show');
set(leg, 'location', 'southoutside');
set(leg, 'orientation', 'horizontal');
title('Rate of opening usage');
print([raw_data '_opening_moves_rate_plot_lin.png']);
