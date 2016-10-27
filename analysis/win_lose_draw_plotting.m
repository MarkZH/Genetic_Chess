clear;
close all;

graphics_toolkit("gnuplot");

[filename, directory, ~] = uigetfile();
raw_data = fullfile(directory, filename);
python('analysis/win_lose_draw_plots.py', ['"' raw_data '"']);

data = importdata([raw_data, '_plots.txt'], '\t');

game = data.data(:, 1);
white_wins = data.data(:, 2);
black_wins = data.data(:, 3);
draws = data.data(:, 4);
time_left = data.data(:, 5);
result_type = data.data(:, 6);


figure('Position', [0, 0, 1200, 1000]);
plot(game, white_wins, ...
   'LineWidth', 3,
	 game, black_wins, ...
   'LineWidth', 3,
	 game, draws, ...
   'LineWidth', 3);
xlabel(data.colheaders{1});
ylabel('Count');
legend(data.colheaders{2}, ...
       data.colheaders{3}, ...
       data.colheaders{4}, ...
       'location', 'northwest');
xlabel('Game number');
ylabel('Count');
title('Count of winning sides');
print([raw_data '_game_outcomes.png']);


checkmates = zeros(size(game));
fifty_moves = zeros(size(game));
threefold = zeros(size(game));
time_out = zeros(size(game));
for index = 1 : length(game)
  if result_type(index) == 0
    checkmates(index) = 1;
  elseif result_type(index) == 1;
    fifty_moves(index) = 1;
  elseif result_type(index) == 2;
    threefold(index) = 1;
  elseif result_type(index) == 3
    time_out(index) = 1;
  else
    disp(['Unknown result type' num2str(result_type(index))]);
  end
end

figure('Position', [0, 0, 1200, 1000]);
plot(game, cumsum(checkmates), ...
   'LineWidth', 3,
     game, cumsum(time_out), ...
   'LineWidth', 3, ...
     game, cumsum(fifty_moves), ...
   'LineWidth', 3,
     game, cumsum(threefold), ...
   'LineWidth', 3);
xlabel('Game number');
ylabel('Count');
legend('Checkmate', 'Time', '50-move', '3-fold', ...
       'location', 'northwest');
title('Type of endgame counts');
print([raw_data '_game_result_type_frequencies.png']);
