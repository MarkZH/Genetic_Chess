clear;
close all;
graphics_toolkit("gnuplot");

[filename, directory, ~] = uigetfile();
gene_pool_filename = fullfile(directory, filename);

if length(gene_pool_filename) == 0
  return
end

disp('Parsing genes with python script ...');
fflush(stdout); % Octave only; comment out for Matlab use
python('gene_pool_analyze.py', gene_pool_filename);
filename = [gene_pool_filename '_parsed.txt'];
disp('Importing parsed data ...');
fflush(stdout); % Octave only; comment out for Matlab use
data = importdata(filename, ',');
id_list = data.data(:, 1);

disp('Plotting ...');
fflush(stdout); % Octave only; comment out for Matlab use
xaxis = data.colheaders(1){1};

piece_strength_prefix = "Piece Strength Gene";
piece_strength_figure = figure('Position', [0, 0, 1200, 1000]);
piece_strength_legend_entries = {};
title('Piece Strength Evolution', 'FontSize', 22);

scalar_figure = figure('Position', [0, 0, 1200, 1000]);
scalar_suffix = "Scalar";
scalar_legend_entries = {};
title('Gene Scalar Evolution', 'FontSize', 22);

for index = 1 : length(data.colheaders)
    if strcmp(data.colheaders(index){1}, 'Total Force Gene - Scalar')
        total_force_scalar_sign = sign(data.data(:, index));
        total_force_scalar_sign(~isfinite(total_force_scalar_sign)) = 0;
        break;
    end
end

for yi = 2 : length(data.colheaders)
  this_data = data.data(:, yi);
  name = data.colheaders(yi){1};

  figure('Position', [0, 0, 1200, 1000]);
  scatter(id_list, this_data);
  xlabel(xaxis, 'FontSize', 18);
  title(name, 'FontSize', 22);
  set(gca, 'FontSize', 14);
  print([gene_pool_filename '_' name '.png']);
  
  % Fill in nan gaps in data
  this_data(~isfinite(this_data)) = 0;

  conv_window = 500;
  plot_figure = nan;
  if name(1:length(piece_strength_prefix)) == piece_strength_prefix
    plot_figure = piece_strength_figure;
    piece_strength_legend_entries{length(piece_strength_legend_entries) + 1} = name(end);
    sign_change = total_force_scalar_sign;
  end
  
  if name(length(name) - length(scalar_suffix) + 1:end) == scalar_suffix
    plot_figure = scalar_figure;
    scalar_legend_entries{length(scalar_legend_entries) + 1} = name;
    sign_change = ones(size(this_data));
  end

  if ~isnan(plot_figure)
    figure(plot_figure);
    hold all;
    smooth_data = conv(this_data.*sign_change, ones(conv_window, 1), 'valid')/conv_window;
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
