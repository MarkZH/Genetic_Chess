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

data = importdata(filename);
move_count = data(:,1);
total_moves = data(:,2);
legal_move_count = data(:,3);


hist3([move_count, legal_move_count], [max(move_count), max(legal_move_count)], 'FaceColor', 'interp');
xlabel('Moves made');
ylabel('Legal move count');
view(2);
print([filename '-move_count.png']);
close all;
return;

game_completion_fraction = floor(100*move_count./total_moves);
hist3([game_completion_fraction, legal_move_count], [100, max(legal_move_count)]);
xlabel('Fraction of game complete');
ylabel('Legal move count');
view(2);
print([filename '-fraction.png']);
