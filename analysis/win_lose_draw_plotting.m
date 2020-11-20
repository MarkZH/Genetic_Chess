warning('off'); % Disable warnings about non-positive data
% in loglog plots (turn back on for debugging)

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
marks_file_name = 0;
file_directory = '';
marks_directory = '';
if isOctave
    graphics_toolkit('gnuplot');
    args = argv();
    for argi = 1 : length(args)
        option = args{argi};
        if isempty(option) || option(1) == '-'
            continue;
        end

        if filename == 0
            filename = option;
        else
            marks_file_name = option;
            break;
        end
    end
end

if filename == 0
    [filename, file_directory, ~] = uigetfile();
    if filename == 0
        return
    end
    if marks_file_name == 0
        [marks_file_name, marks_directory, ~] = uigetfile();
    end
end

raw_data = fullfile(file_directory, filename);
if isOctave
    python('analysis/win_lose_draw_plots.py', ['"' raw_data '"']);
end
data = importdata([raw_data, '_plots.txt'], '\t');

game_number_marks = [];
if marks_file_name != 0
    marks_file_name = fullfile(marks_directory, marks_file_name);
    marks_data = importdata(marks_file_name, ';', 1);
    game_number_marks = marks_data.data(:,2)';
    game_notes = marks_data.textdata(2:end);
end

game_number = data.data(:, 1);
white_wins = data.data(:, 2);
black_wins = data.data(:, 3);
draws = data.data(:, 4);
game_time = data.data(:, 5);
result_type = data.data(:, 6);
white_time_left = data.data(:, 7);
black_time_left = data.data(:, 8);
moves_in_game = data.data(:, 9);


figure;
hold all;
semilogx(game_number, 100*white_wins./game_number, 'LineWidth', 3, ...
         game_number, 100*black_wins./game_number, 'LineWidth', 3, ...
         game_number, 100*draws./game_number, 'LineWidth', 3);
xlabel(data.colheaders{1});
ylabel('Percentage');
legend([data.colheaders{2} ' (' num2str(white_wins(end)) ')'], ...
       [data.colheaders{3} ' (' num2str(black_wins(end)) ')'], ...
       [data.colheaders{4} ' (' num2str(draws(end)) ')'], ...
       'location', 'northeast');
title('Winning Sides');

for index = 1:length(game_number_marks)
    plot(game_number_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
end

print([raw_data '_game_outcomes.png']);
close;



white_checkmates = result_type == 0;
black_checkmates = result_type == 1;
fifty_moves = result_type == 2;
threefold = result_type == 3;
white_time_win = result_type == 4;
black_time_win = result_type == 5;
material = result_type == 6;
no_legal = result_type == 7;
time_and_material = result_type == 8;
number_of_games = length(game_number);
if any(result_type > 8 | result_type < 0)
    disp('Unknown result types found.');
end

figure;
hold all;
loglog(game_number, 100*cumsum(white_checkmates)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(black_checkmates)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(white_time_win)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(black_time_win)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(fifty_moves)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(threefold)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(material)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(no_legal)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(time_and_material)./game_number, 'LineWidth', 3);
xlabel('Games played');
ylabel('Percentage');
legend(['White checkmate (' num2str(sum(white_checkmates)) ')'], ...
       ['Black checkmate (' num2str(sum(black_checkmates)) ')'], ...
       ['White wins on time (' num2str(sum(white_time_win)) ')'], ...
       ['Black wins on time (' num2str(sum(black_time_win)) ')'], ...
       ['50-move (' num2str(sum(fifty_moves)) ')'], ...
       ['3-fold (' num2str(sum(threefold)) ')'], ...
       ['Insufficient material (' num2str(sum(material)) ')'], ...
       ['Stalemate (' num2str(sum(no_legal)) ')'], ...
       ['Time expires w/o material (' num2str(sum(time_and_material)) ')'], ...
       'location', 'southwest');
title('Type of Endgame');

for index = 1:length(game_number_marks)
    plot(game_number_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
end

print([raw_data '_game_result_type_frequencies.png']);
close;

if max(game_time) > 0
    figure;
    hold all;

    max_time_left = max(max(white_time_left), max(black_time_left));
    below_zero = -0.05*max_time_left;
    below_zero_random = below_zero*(1.5 - rand(size(white_time_left)));
    white_time_left(white_time_left < 0) = below_zero_random(white_time_left < 0);
    black_time_left(black_time_left < 0) = below_zero_random(black_time_left < 0);

    scatter(game_number, white_time_left, 'k');
    scatter(game_number, black_time_left, 'k');
    window = 1000;
    x_margin = floor(window/2);
    avg_x_axis = game_number(x_margin : end - x_margin);
    avg_time_left = (white_time_left + black_time_left)/2;
    plot(avg_x_axis,
         movmean(avg_time_left, window, 'endpoints', 'discard'),
         'r',
         'displayname', 'Moving average',
         'linewidth', 3);
    leg = legend('show');
    set(leg, 'location', 'northwest');
    ylim(max_time_left*[-0.10, 1.05]);
    xlabel('Game number');
    ylabel('Time (sec)');
    title('Time left on clock at end of game');

    if length(game_number_marks) > 0
        for index = 1:length(game_number_marks)
            plot(game_number_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
        end

        legend('location', 'southoutside', 'orientation', 'horizontal');
    end

    print([raw_data '_game_time_left.png']);
    close;
end


% Don't plot top 0.1% of longest games to make trends easier to see
max_game_count = floor(0.999*length(moves_in_game));
[counts, bins] = hist(moves_in_game, (1 : max(moves_in_game)));
total_counts = 0;
for index = 1 : length(counts)
    total_counts = total_counts + counts(index);
    if total_counts > max_game_count
        max_game_length_display = bins(index);
        break;
    end
end

figure;
hold all;
scatter(game_number, moves_in_game, 'k');
xlabel('Game number');
ylabel('Moves in Game');
title('Number of moves in game');
ylim([0, max_game_length_display]);

if length(game_number_marks) > 0
    for index = 1:length(game_number_marks)
        plot(game_number_marks(index)*[1 1], ylim, 'displayname', game_notes{index});
    end

    legend('location', 'southoutside', 'orientation', 'horizontal');
end

print([raw_data '_moves_in_game.png']);
close;

figure;
hold all;
bar(bins, counts, 'barwidth', 1, 'facecolor', 'y');
xlabel('Moves in Game');
ylabel(['Counts (total = ' num2str(number_of_games) ')']);
title('Number of moves in game');
xlim([0, max_game_length_display]);
set(gca, 'xtick', 0 : 10 : max_game_length_display);
mean_moves = mean(moves_in_game);
mode_moves = mode(moves_in_game);
std_dev = std(moves_in_game);

% Log-normal fit
mean_log = mean(log(moves_in_game));
std_log = std(log(moves_in_game));
fit = number_of_games*exp(-.5*((log(bins) - mean_log)/std_log).^2)./(bins*std_log*sqrt(2*pi));
plot(bins, fit, 'linewidth', 3);

last10p = [floor(.9*length(moves_in_game)) : length(moves_in_game)];
mean_log10p = mean(log(moves_in_game(last10p)));
std_log10p = std(log(moves_in_game(last10p)));
fit10p = number_of_games*exp(-.5*((log(bins) - mean_log10p)/std_log10p).^2)./(bins*std_log10p*sqrt(2*pi));
plot(bins, fit10p, 'k', 'linewidth', 3);

legend('Histogram', 'Log-Normal distribution (all games)', 'Log-normal distribution (last 10%)');

stats = {['Mean = ' num2str(mean_moves)], ...
         ['Median = ' num2str(median(moves_in_game))], ...
         ['Mode = ' num2str(mode_moves)], ...
         ['Std. Dev. = ' num2str(std_dev)], ...
         ['Min = ' num2str(min(moves_in_game))], ...
         ['Max = ' num2str(max(moves_in_game))], ...
         [''], ...
         ['Log-Norm Peak (all) = ' num2str(exp(mean_log - std_log^2))], ...
         ['Log-Norm Width (all) = ' num2str(std_log)], ...
         [''], ...
         ['Log-Norm Peak (last 10%) = ' num2str(exp(mean_log10p - std_log10p^2))], ...
         ['Log-Norm Width (last 10%) = ' num2str(std_log10p)]};

xl = xlim;
yl = ylim;
text(0.5*xl(2), 0.5*yl(2), stats);

print([raw_data '_moves_in_game_histogram.png']);
close;
