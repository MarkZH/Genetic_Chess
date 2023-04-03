warning('off');

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
    args = argv();
    if length(args) > 0
        filename = args{1};
        plot_title = args{2};
    end
end

if filename == 0
    [filename, directory, ~] = uigetfile();
end

if filename == 0
    return
end
raw_data = fullfile(directory, filename);

if isOctave
    python('analysis/opening_plot.py', ['"' raw_data '"']);
end

top_data = importdata([raw_data, '_top_opening_data.txt'], ',');

figure;
hold all;
game_counts = (1:size(top_data.data, 1))';
ymax = 0;
for col = 1 : size(top_data.data, 2)
    opening_counts = cumsum(top_data.data(:, col));
    percents = 100*opening_counts./game_counts;
    plot(percents, 'LineWidth', 2, 'displayname', top_data.colheaders{col});
    max_percent = max(percents(ceil(0.01*length(percents) : end)));
    ymax = max([ymax max_percent]);
end

xlabel('Games played');
ylabel('Percent of games');
ylim([0 ymax]);
leg = legend('show');
set(leg, 'location', 'eastoutside');
title(plot_title);
print([raw_data '_opening_moves_plot.png']);
close;
