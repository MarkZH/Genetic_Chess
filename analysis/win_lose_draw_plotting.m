raw_data = uigetfile;
python('win_lose_draw_plots.py', raw_data);

data = importdata([raw_data, '_plots.txt'], '\t');

game = data.data(:, 1);
white_wins = data.data(:, 2);
black_wins = data.data(:, 3);
draws = data.data(:, 4);
total_wins = white_wins + black_wins;
time_left = data.data(:, 5);
result_type = data.data(:, 6);

figure;
plot(game, white_wins, ...
	 game, black_wins, ...
	 game, total_wins, ...
	 game, draws);

xlabel(data.colheaders{1});
ylabel('Count');

legend(data.colheaders{2}, ...
       data.colheaders{3}, ...
	   [data.colheaders{2} ' + ' data.colheaders{3}], ...
	   data.colheaders{4});
     
figure;
n = 50;
window = ones(n, 1)/n;
plot(game(1:end-1), diff(conv(white_wins, window, 'same')), ...
     game(1:end-1), diff(conv(black_wins, window, 'same')), ...
     game(1:end-1), diff(conv(total_wins, window, 'same')), ...
     game(1:end-1), diff(conv(draws, window, 'same')));
legend(data.colheaders{2}, ...
       data.colheaders{3}, ...
	   [data.colheaders{2} ' + ' data.colheaders{3}], ...
	   data.colheaders{4});
title('Rate of Increase');
ylim([0, 1.1]);

figure;
plot(game, total_wins, ...
	   game, draws);

xlabel(data.colheaders{1});
ylabel('Count');

legend([data.colheaders{2} ' + ' data.colheaders{3}], ...
	     data.colheaders{4});
     
figure;
n = 50;
window = ones(n, 1)/n;
plot(game(1:end-1), diff(conv(total_wins, window, 'same')), ...
     game(1:end-1), diff(conv(draws, window, 'same')));
legend([data.colheaders{2} ' + ' data.colheaders{3}], ...
	     data.colheaders{4});
title('Rate of Increase');
ylim([0, 1.1]);

checkmates = zeros(size(game));
fifty_moves = zeros(size(game));
threefold = zeros(size(game));
time_out = zeros(size(game));
for index = 1 : length(game)
  if result_type(index) == 0
    checkmate(index) = 1;
  elseif result_type(index) == 1;
    fifty_moves(index) = 1;
  elseif result_type(index) == 2;
    threefold(index) = 1;
  else
    time_out(index) = 1;
  end
end

figure;
plot(game, cumsum(checkmate), ...
     game, cumsum(fifty_moves), ...
     game, cumsum(threefold), ...
     game, cumsum(time_out));
legend('Checkmate', '50-move', '3-fold', 'Time');
