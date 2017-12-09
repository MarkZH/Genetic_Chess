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
    game_number_marks = [];
    if length(args) > 1
      marks_file_name = args{2};
      if ~isempty(marks_file_name)
        game_number_marks = importdata(marks_file_name)';
      end
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
hold all;
semilogx(game_number, 100*white_wins./game_number, 'LineWidth', 3, ...
         game_number, 100*black_wins./game_number, 'LineWidth', 3, ...
         game_number, 100*draws./game_number, 'LineWidth', 3);
xlabel(data.colheaders{1});
ylabel('Percentage');
legend(data.colheaders{2}, ...
       data.colheaders{3}, ...
       data.colheaders{4}, ...
       'location', 'northeast');
title('Winning Sides');

for n = game_number_marks
  plot(n*[1 1], ylim);
end

print([raw_data '_game_outcomes.png']);



white_checkmates = zeros(size(game_number));
black_checkmates = zeros(size(game_number));
fifty_moves = zeros(size(game_number));
threefold = zeros(size(game_number));
white_time_win = zeros(size(game_number));
black_time_win = zeros(size(game_number));
material = zeros(size(game_number));
no_legal = zeros(size(game_number));
number_of_games = length(game_number);
for index = 1 : number_of_games
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
hold all;
loglog(game_number, 100*cumsum(white_checkmates)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(black_checkmates)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(white_time_win)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(black_time_win)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(fifty_moves)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(threefold)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(material)./game_number, 'LineWidth', 3, ...
       game_number, 100*cumsum(no_legal)./game_number, 'LineWidth', 3);
xlabel('Games played');
ylabel('Percentage');
legend('White checkmate', 'Black checkmate', ...
       'White wins on time', 'Black wins on time', ...
       '50-move', '3-fold', 'Insufficient material', 'Stalemate', ...
       'location', 'southwest');
title('Type of Endgame');

for n = game_number_marks
  plot(n*[1 1], ylim);
end

print([raw_data '_game_result_type_frequencies.png']);

if max(game_time) > 0
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

  for n = game_number_marks
    plot(n*[1 1], ylim);
  end

  print([raw_data '_game_time_left.png']);

  figure('Position', [0, 0, 1200, 1000]);
  all_time_left = [black_time_left; white_time_left]./[game_time; game_time];
  all_time_left(all_time_left < 0) = -.05;
  hist(all_time_left, 100);
  xlabel('Fraction of time left on clock');
  ylabel(['Counts (total = ' num2str(number_of_games) ')']);
  title('Time left on clock at end of game')
  print([raw_data '_game_time_left_histogram.png']);
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

figure('Position', [0, 0, 1200, 1000]);
hold all;
scatter(game_number, moves_in_game, 'k');
xlabel('Game number');
ylabel('Moves in Game');
title('Number of moves in game')
ylim([0, max_game_length_display]);

for n = game_number_marks
  plot(n*[1 1], ylim);
end

print([raw_data '_moves_in_game.png']);

figure('Position', [0, 0, 1200, 1000]);
hold all;
bar(bins, counts, 'barwidth', 1);
xlabel('Moves in Game');
ylabel(['Counts (total = ' num2str(number_of_games) ')']);
title('Number of moves in game')
xlim([0, max_game_length_display]);
set(gca, 'xtick', 0 : 10 : max_game_length_display);
mean_moves = mean(moves_in_game);
mode_moves = mode(moves_in_game);
std_dev = std(moves_in_game);

% Log-normal fit
mean_log = sum(log(bins).*counts)/sum(counts);
std_log = sqrt(sum(((log(bins) - mean_log).^2).*counts)/sum(counts));

fit = sum(counts)*exp(-.5*((log(bins) - mean_log)/std_log).^2)./(bins*std_log*sqrt(2*pi));
plot(bins, fit, 'linewidth', 3);

legend('Histogram', 'Log-Normal distribution');

stats = {['Mean = ' num2str(mean_moves)], ...
         ['Median = ' num2str(median(moves_in_game))], ...
         ['Mode = ' num2str(mode_moves)], ...
         ['\sigma = ' num2str(std_dev)], ...
         ['Log-Norm Peak = ' num2str(exp(mean_log))] ...
         ['Log-Norm Width = ' num2str(std_log)] ...
         ['Min = ' num2str(min(moves_in_game))], ...
         ['Max = ' num2str(max(moves_in_game))]};
xl = xlim;
yl = ylim;
text(0.7*xl(2), 0.6*yl(2), stats, 'fontsize', 14);

print([raw_data '_moves_in_game_histogram.png']);
