% Plots the number of legal moves after N moves or after
% the fraction of game completed (moves/total moves).

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
    graphics_toolkit('gnuplot');
    pkg load statistics;
    args = argv();
    if length(args) > 0
        filename = args{1};
    else
        [filename, directory, ~] = uigetfile();
    end
end

if filename == 0
    return
end

filename = fullfile(directory, filename);

disp('Reading data file ...');
data = importdata(filename);
move_count = data(:,1);
total_moves = data(:,2);
legal_move_count = data(:,3);

game_count = sum(move_count == total_moves);

max_game_length = 200;
usable_range = (total_moves < max_game_length);
move_count = move_count(usable_range);
total_moves = total_moves(usable_range);
legal_move_count = legal_move_count(usable_range);
game_completion_fraction = floor(100*move_count./total_moves);

disp(['Game count = ' num2str(game_count)]);
disp(['Data point count = ' num2str(length(move_count))]);


disp('Creating count heat map ...');
heat_map_count = zeros(max(legal_move_count) + 1, max(move_count));
for index = 1 : length(move_count)
    heat_map_count(legal_move_count(index) + 1, move_count(index)) += 1;
end
heat_map_count = heat_map_count/game_count;
heat_map_count(heat_map_count > 0.05) = 0.05;
disp('Drawing count heat map ...');
contourf(min(move_count):max(move_count), min(legal_move_count):max(legal_move_count),  heat_map_count, 100);
xlabel('Moves made');
ylabel('Legal move count');
colormap('hot');
colorbar;
print([filename '-move_count.png']);


disp('Creating fraction heat map ...');
heat_map_fraction = zeros(max(legal_move_count) + 1, 101);
for index = 1 : length(move_count)
    heat_map_fraction(legal_move_count(index) + 1, game_completion_fraction(index) + 1) += 1;
end
heat_map_fraction = heat_map_fraction/game_count;
heat_map_fraction(heat_map_fraction > 0.07) = 0.07;
disp('Drawing fraction heat map ...');
contourf(0:100, min(legal_move_count):max(legal_move_count), heat_map_fraction, 100);
xlabel('Fraction of game complete');
ylabel('Legal move count');
colormap('hot');
colorbar;
print([filename '-fraction.png']);
