clear;
close all;
graphics_toolkit("gnuplot");

[filename, directory, ~] = uigetfile();
gene_pool_filename = fullfile(directory, filename);

if length(gene_pool_filename) == 0
  return
end

python('gene_pool_analyze.py', gene_pool_filename);
filename = [gene_pool_filename '_parsed.txt'];
data = importdata(filename, ',');
id_list = data.data(:, 1);

xaxis = data.colheaders(1){1};

piece_strength_prefix = "Piece Strength Gene";
piece_strength_figure = figure('Position', [0, 0, 1200, 1000]);
piece_strength_legend_entries = {};
title('Piece Strength Evolution', 'FontSize', 22);

scalar_figure = figure('Position', [0, 0, 1200, 1000]);
scalar_suffix = "Scalar";
scalar_legend_entries = {};
title('Gene Scalar Evolution', 'FontSize', 22);

for yi = 2 : length(data.colheaders)
  this_data = data.data(:, yi);
  name = data.colheaders(yi){1};

  figure('Position', [0, 0, 1200, 1000]);
  scatter(id_list, this_data);
  xlabel(xaxis, 'FontSize', 18);
  title(name, 'FontSize', 22);
  set(gca, 'FontSize', 14);
  print([gene_pool_filename '_' name '.png']);
  
  conv_window = 500;
  if name(1:length(piece_strength_prefix)) == piece_strength_prefix
    figure(piece_strength_figure);
    hold all;
    plot(conv(this_data, ones(conv_window, 1), 'valid')/conv_window, 'LineWidth', 3);
    piece_strength_legend_entries{length(piece_strength_legend_entries) + 1} = name;
  end
  
  if name(length(name) - length(scalar_suffix) + 1:end) == scalar_suffix
    figure(scalar_figure);
    hold all;
    plot(conv(this_data, ones(conv_window, 1), 'valid')/conv_window, 'LineWidth', 3);
    scalar_legend_entries{length(scalar_legend_entries) + 1} = name;
  end
end

figure(piece_strength_figure);
legend(piece_strength_legend_entries, 'Location', 'northwest');
print([gene_pool_filename '_piece_strength.png']);

figure(scalar_figure);
legend(scalar_legend_entries, 'Location', 'northwest');
print([gene_pool_filename '_gene_scalars.png']);