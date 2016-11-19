clear;
close all;

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

if isOctave
  graphics_toolkit('gnuplot'); % Octave only
end

[filename, directory, ~] = uigetfile();
if filename == 0
  return
end
gene_pool_filename = fullfile(directory, filename);

if isOctave
  disp('Parsing genes with python script ...');
  fflush(stdout); % Octave only
  python('analysis/gene_pool_analyze.py', gene_pool_filename);
end

filename = [gene_pool_filename '_parsed.txt'];
disp('Importing parsed data ...');
if isOctave
  fflush(stdout); % Octave only
end

data = importdata(filename, ',');
id_list = data.data(:, 1);

disp('Plotting ...');

if isOctave
  fflush(stdout); % Octave only; comment out for Matlab use
end

xaxis_list = data.colheaders(1);
xaxis = xaxis_list{1};

piece_strength_prefix = 'Piece Strength Gene';
piece_strength_figure = figure('Position', [0, 0, 1200, 1000]);
piece_strength_legend_entries = {};
title('Piece Strength Evolution', 'FontSize', 22);

scalar_figure = figure('Position', [0, 0, 1200, 1000]);
scalar_suffix = 'Scalar';
scalar_legend_entries = {};
title('Gene Scalar Evolution', 'FontSize', 22);

for yi = 2 : length(data.colheaders)
  this_data = data.data(:, yi);
  name_list = data.colheaders(yi);
  name = name_list{1};

  figure('Position', [0, 0, 1200, 1000]);
  scatter(id_list, this_data);
  xlabel(xaxis, 'FontSize', 18);
  title(name, 'FontSize', 22);
  set(gca, 'FontSize', 14);
  print([gene_pool_filename '_' name '.png']);
  
  % Fill in nan gaps in data
  this_data(~isfinite(this_data)) = 0;

  conv_window = 100;
  plot_figure = nan;
  if name(1:length(piece_strength_prefix)) == piece_strength_prefix
    plot_figure = piece_strength_figure;
    piece_strength_legend_entries{length(piece_strength_legend_entries) + 1} = name(end);
  end
  
  if name(length(name) - length(scalar_suffix) + 1:end) == scalar_suffix
    plot_figure = scalar_figure;
    scalar_legend_entries{length(scalar_legend_entries) + 1} = name;
  end

  if ~isnan(plot_figure)
    figure(plot_figure);
    hold all;
    smooth_data = conv(this_data, ones(conv_window, 1), 'valid')/conv_window;
    plot(smooth_data, 'LineWidth', 3);
  end
end

figure(piece_strength_figure);
legend(piece_strength_legend_entries, 'Location', 'northwest');
print([gene_pool_filename '_piece_strength.png']);

figure(scalar_figure);
legend(scalar_legend_entries, 'Location', 'northwest');
print([gene_pool_filename '_gene_scalars.png']);
disp('Done.');
