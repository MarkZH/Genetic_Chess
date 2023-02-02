warning('off'); % Disable warnings about non-positive data
% in loglog plots (turn back on for debugging)

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
file_directory = '';
if isOctave
    args = argv();
    if length(args) > 0
        filename = args{1};
    end
end

if filename == 0
    [filename, file_directory, ~] = uigetfile();
    if filename == 0
        return
    end
end

raw_data = fullfile(file_directory, filename);
if isOctave
    python('analysis/win_lose_draw_plots.py', ['"' raw_data '"']);
end
data = importdata([raw_data, '_plots.txt'], '\t');

game_number = data.data(:, 1);
white_wins = data.data(:, 2);
black_wins = data.data(:, 3);
draws = data.data(:, 4);
game_time = data.data(:, 5);
result_type = data.data(:, 6);
white_time_left = data.data(:, 7);
black_time_left = data.data(:, 8);
moves_in_game = data.data(:, 9);

line_width = 2;
draw_bar_line_width = 3;
marker_size = 5;
stat_text_size = 7;

figure;
hold all;
semilogx(game_number, 100*white_wins./game_number, 'LineWidth', line_width, ...
         game_number, 100*black_wins./game_number, 'LineWidth', line_width, ...
         game_number, 100*draws./game_number, 'LineWidth', line_width);
xlabel(data.colheaders{1});
ylabel('Percentage');
legend([data.colheaders{2} ' (' num2str(white_wins(end)) ')'], ...
       [data.colheaders{3} ' (' num2str(black_wins(end)) ')'], ...
       [data.colheaders{4} ' (' num2str(draws(end)) ')'], ...
       'location', 'northeast');
title('Winning Sides');

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
loglog(game_number, 100*cumsum(white_checkmates)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(black_checkmates)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(white_time_win)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(black_time_win)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(fifty_moves)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(threefold)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(material)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(no_legal)./game_number, 'LineWidth', line_width, ...
       game_number, 100*cumsum(time_and_material)./game_number, 'LineWidth', line_width);
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
       'fontsize', 7, ...
       'location', 'southwest');
title('Type of Endgame');

print([raw_data '_game_result_type_frequencies.png']);
close;

if max(game_time) > 0
    figure;
    hold all;

    avg_time_left = (white_time_left + black_time_left)/2;
    max_time_left = max(max(white_time_left), max(black_time_left));
    below_zero = -0.05*max_time_left;
    below_zero_random = below_zero*(1.5 - rand(size(white_time_left)));
    white_time_left(white_time_left < 0) = below_zero_random(white_time_left < 0);
    black_time_left(black_time_left < 0) = below_zero_random(black_time_left < 0);

    plot(game_number, white_time_left, '.k', 'markersize', marker_size);
    plot(game_number, black_time_left, '.k', 'markersize', marker_size);
    window = 100;
    x_margin = floor(window/2);
    avg_x_axis = game_number(x_margin : end - x_margin);
    p = plot(avg_x_axis,
             movmean(avg_time_left, window, 'endpoints', 'discard'),
             'r',
             'linewidth', line_width);
    leg = legend(p, 'Moving average');
    set(leg, 'location', 'northwest');
    ylim(max_time_left*[-0.10, 1.05]);
    xlabel('Game number');
    ylabel('Time (sec)');
    title('Time left on clock at end of game');

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
plot(game_number, moves_in_game, '.k', 'markersize', marker_size);
xlabel('Game number');
ylabel('Moves in Game');
title('Number of moves in game');
ylim([0, max_game_length_display]);

print([raw_data '_moves_in_game.png']);
close;

figure;
hold all;
bar(bins, counts, 'barwidth', 1, 'facecolor', 'w', 'displayname', 'All game lengths');
xlabel('Moves in Game');
ylabel(['Counts (total = ' num2str(number_of_games) ')']);
title('Number of moves in game');
xlim([0, max_game_length_display]);
set(gca, 'xtick', 0 : 10 : max_game_length_display);
mean_moves = mean(moves_in_game);
mode_moves = mode(moves_in_game);
std_dev = std(moves_in_game);

% Log-normal fit
moves_in_game_fit = moves_in_game(moves_in_game > 0);
bins_fit = bins(bins > 0);
mean_log = mean(log(moves_in_game_fit));
std_log = std(log(moves_in_game_fit));
fit = number_of_games*exp(-.5*((log(bins_fit) - mean_log)/std_log).^2)./(bins_fit*std_log*sqrt(2*pi));
plot(bins_fit, fit, 'linewidth', line_width, 'displayname', 'Log-Normal fit (all games)');

legend show;

stats = {['Mean = ' num2str(mean_moves)], ...
         ['Median = ' num2str(median(moves_in_game))], ...
         ['Mode = ' num2str(mode_moves)], ...
         ['Std. Dev. = ' num2str(std_dev)], ...
         ['Min = ' num2str(min(moves_in_game))], ...
         ['Max = ' num2str(max(moves_in_game))], ...
         [''], ...
         ['Log-Norm Peak (all) = ' num2str(exp(mean_log - std_log^2))], ...
         ['Log-Norm Width (all) = ' num2str(std_log)]};

xl = xlim;
yl = ylim;
text(0.65*xl(2), 0.5*yl(2), stats, 'fontsize', stat_text_size);

print([raw_data '_moves_in_game_histogram.png']);
close;

winning_games_lengths = moves_in_game(white_checkmates | black_checkmates);
[counts, bins] = hist(winning_games_lengths, (1 : max(moves_in_game)));
figure;
hold all;
bar(bins, counts, 'barwidth', 1, 'facecolor', 'w', 'displayname', 'All checkmates');
title('Checkmate game lengths');
xlim([0, max_game_length_display]);
set(gca, 'xtick', 0 : 10 : max_game_length_display);
% Log-normal fit
bins_fit = bins(bins > 0);
mean_log = mean(log(winning_games_lengths));
std_log = std(log(winning_games_lengths));
winning_games_count = length(winning_games_lengths);
fit = winning_games_count*exp(-.5*((log(bins_fit) - mean_log)/std_log).^2)./(bins_fit*std_log*sqrt(2*pi));
plot(bins_fit, fit, 'linewidth', line_width, 'displayname', 'Log-normal fit');

xlabel('Moves in Game');
ylabel(['Counts (total = ' num2str(winning_games_count) ')']);

stats = {['Mean = ' num2str(mean(winning_games_lengths))], ...
         ['Median = ' num2str(median(winning_games_lengths))], ...
         ['Mode = ' num2str(mode(winning_games_lengths))], ...
         ['Std. Dev. = ' num2str(std(winning_games_lengths))], ...
         ['Min = ' num2str(min(winning_games_lengths))], ...
         ['Max = ' num2str(max(winning_games_lengths))], ...
         [''], ...
         ['Log-Norm Peak = ' num2str(exp(mean_log - std_log^2))], ...
         ['Log-Norm Width = ' num2str(std_log)]};

xl = xlim;
yl = ylim;
text(0.65*xl(2), 0.5*yl(2), stats, 'fontsize', stat_text_size);

legend show;

print([raw_data '_moves_in_game_histogram_checkmate.png']);


drawn_games = (fifty_moves | threefold | material | no_legal);
[drawn_counts, drawn_bins] = hist(moves_in_game(drawn_games), (1 : max(moves_in_game)));
[fifty_counts, fifty_bins] = hist(moves_in_game(fifty_moves), (1 : max(moves_in_game)));
[threefold_counts, threefold_bins] = hist(moves_in_game(threefold), (1 : max(moves_in_game)));
[material_counts, material_bins] = hist(moves_in_game(material), (1 : max(moves_in_game)));
[no_legal_counts, no_legal_bins] = hist(moves_in_game(no_legal), (1 : max(moves_in_game)));
figure;
hold all;
bar(drawn_bins, drawn_counts, 'barwidth', 1, 'facecolor', 'w', 'displayname', 'All draws');
plot(fifty_bins, fifty_counts, 'displayname', 'Fifty moves', 'linewidth', draw_bar_line_width);
plot(threefold_bins, threefold_counts, 'displayname', '3-fold', 'linewidth', draw_bar_line_width, 'color', 'blue');
plot(material_bins, material_counts, 'k', 'displayname', 'Material', 'linewidth', draw_bar_line_width);
plot(no_legal_bins, no_legal_counts, 'g', 'displayname', 'Stalemate', 'linewidth', draw_bar_line_width);
title ('Draw game lengths');
xlabel('Moves in Game');
ylabel(['Counts (total = ' num2str(sum(drawn_games)) ')']);
legend show;
xlim([0, max_game_length_display]);
set(gca, 'xtick', 0 : 10 : max_game_length_display);
print([raw_data '_moves_in_game_histogram_draw.png']);

timeout_games = (white_time_win | black_time_win | time_and_material);
[counts, bins] = hist(moves_in_game(timeout_games), (1 : max(moves_in_game)));
figure;
bar(bins, counts, 'barwidth', 1, 'facecolor', 'w');
title ('Timeout game lengths');
xlabel('Moves in Game');
ylabel(['Counts (total = ' num2str(sum(timeout_games)) ')']);
xlim([0, max_game_length_display]);
set(gca, 'xtick', 0 : 10 : max_game_length_display);
print([raw_data '_moves_in_game_histogram_timeout.png']);
