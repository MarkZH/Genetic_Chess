warning('off'); % Disable warnings about non-positive data
                % in loglog plots (turn back on for debugging)

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
raw_data = fullfile(directory, filename);

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


figure('Position', [0, 0, 1200, 1000]);
plot(game_number, white_wins, 'LineWidth', 3, ...
	 game_number, black_wins, 'LineWidth', 3, ...
	 game_number, draws,      'LineWidth', 3);
xlabel(data.colheaders{1});
ylabel('Total Counts');
legend(data.colheaders{2}, ...
       data.colheaders{3}, ...
       data.colheaders{4}, ...
       'location', 'northwest');
title('Count of winning sides');
print([raw_data '_game_outcomes_lin.png']);



white_checkmates = zeros(size(game_number));
black_checkmates = zeros(size(game_number));
fifty_moves = zeros(size(game_number));
threefold = zeros(size(game_number));
white_time_win = zeros(size(game_number));
black_time_win = zeros(size(game_number));
material = zeros(size(game_number));
no_legal = zeros(size(game_number));
for index = 1 : length(game_number)
  if result_type(index) == 0
    white_checkmates(index) = 1;
  elseif result_type(index) == 1;
    black_checkmates(index) = 1;
  elseif result_type(index) == 2;
    fifty_moves(index) = 1;
  elseif result_type(index) == 3;
    threefold(index) = 1;
  elseif result_type(index) == 4
    white_time_win(index) = 1;
  elseif result_type(index) == 5
    black_time_win(index) = 1;
  elseif result_type(index) == 6
    material(index) = 1;
  elseif result_type(index) == 7
    no_legal(index) = 1;
  else
    disp(['Unknown result type' num2str(result_type(index))]);
  end
end

figure('Position', [0, 0, 1200, 1000]);
loglog(game_number, cumsum(white_checkmates),  'LineWidth', 3, ...
       game_number, cumsum(black_checkmates),  'LineWidth', 3, ...
       game_number, cumsum(white_time_win),    'LineWidth', 3, ...
       game_number, cumsum(black_time_win),    'LineWidth', 3, ...
       game_number, cumsum(fifty_moves), 'LineWidth', 3, ...
       game_number, cumsum(threefold),   'LineWidth', 3, ...
       game_number, cumsum(material),    'LineWidth', 3, ...
       game_number, cumsum(no_legal),    'LineWidth', 3);
xlabel('Games played');
ylabel('Count');
legend('White checkmate', 'Black checkmate', ...
       'White wins on time', 'Black wins on time', ...
       '50-move', '3-fold', 'Insufficient material', 'Stalemate', ...
       'location', 'northwest');
title('Type of endgame counts');
print([raw_data '_game_result_type_frequencies_log.png']);


figure('Position', [0, 0, 1200, 1000]);
hold all;
white_time_left(white_time_left < 0) = -0.05*max(white_time_left);
black_time_left(black_time_left < 0) = -0.05*max(white_time_left);
scatter(game_number, white_time_left, 'k');
scatter(game_number, black_time_left, 'k');
ylim(max(white_time_left)*[-0.10, 1.05]);
xlabel('Game number');
ylabel('Time left on clock');
title('Time left on clock at end of game')
print([raw_data '_game_time_left.png']);

figure('Position', [0, 0, 1200, 1000]);
hist([black_time_left; white_time_left], 300);
xlabel('Time left on clock');
ylabel(['Counts (total = ' num2str(length(game_number)) ')']);
title('Time left on clock at end of game')
print([raw_data '_game_time_left_histogram.png']);


% Don't plot top 0.1% of longest games to make trends easier to see
[counts, bins] = hist(moves_in_game, (0 : max(moves_in_game)));
total_counts = 0;
max_game_count = floor(0.999*length(moves_in_game));
for index = 1 : length(counts)
  total_counts = total_counts + counts(index);
  if total_counts > max_game_count
    max_game_length_display = bins(index);
    break;
  end
end

figure('Position', [0, 0, 1200, 1000]);
scatter(game_number, moves_in_game);
xlabel('Game number');
ylabel('Moves in Game');
title('Number of moves in game')
ylim([0, max_game_length_display]);
print([raw_data '_moves_in_game.png']);

figure('Position', [0, 0, 1200, 1000]);
bar(bins, counts, 'barwidth', 1);
xlabel('Moves in Game');
ylabel(['Counts (total = ' num2str(length(game_number)) ')']);
title('Number of moves in game')
xlim([0, max_game_length_display]);
set(gca, 'xtick', 0 : 10 : max_game_length_display);
stats = {['Mean = ' num2str(mean(moves_in_game))], ...
         ['Median = ' num2str(std(moves_in_game))], ...
         ['Mode = ' num2str(mode(moves_in_game))], ...
         ['\sigma = ' num2str(median(moves_in_game))], ...
         ['Min = ' num2str(min(moves_in_game))], ...
         ['Max = ' num2str(max(moves_in_game))]};
xl = xlim;
yl = ylim;
text(0.7*xl(2), 0.8*yl(2), stats, 'fontsize', 14);
print([raw_data '_moves_in_game_histogram.png']);
