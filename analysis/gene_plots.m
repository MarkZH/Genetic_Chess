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
  python('analysis/gene_pool_analyze.py', ['"' gene_pool_filename '"']);
end

filename = [gene_pool_filename '_parsed.txt'];

data = importdata(filename, ',');
id_list = data.data(:, 1);
still_alive = logical(data.data(:, end - 1));
pool_ids = data.data(:, end);

xaxis_list = data.colheaders(1);
xaxis = xaxis_list{1};

piece_strength_prefix = 'Piece Strength Gene';
piece_strength_figure = figure('Position', [0, 0, 1200, 1000]);
title('Piece Strength Evolution', 'FontSize', 22);

scalar_figure = figure('Position', [0, 0, 1200, 1000]);
scalar_suffix = ' Gene - Scalar';
title('Gene Scalar Evolution', 'FontSize', 22);

piece_scalar_plots = [false, false];

for yi = 2 : length(data.colheaders) - 2
  this_data = data.data(:, yi);
  name_list = data.colheaders(yi);
  name = name_list{1};

  figure('Position', [0, 0, 1200, 1000]);
  hold all;
  for pool_id = 0 : max(pool_ids)
    scatter(id_list(pool_ids == pool_id), this_data(pool_ids == pool_id), 8, 'filled');
    h = scatter(id_list(still_alive & pool_ids == pool_id), this_data(still_alive & pool_ids == pool_id), 15, 'k');
    set(h, 'linewidth', 1)
  end
  xlabel(xaxis, 'FontSize', 18);
  title(name, 'FontSize', 22);
  set(gca, 'FontSize', 14);
  print([gene_pool_filename '_' name '.png']);
  
  % Fill in nan gaps in data
  this_data(~isfinite(this_data)) = 0;

  plot_figure = nan;
  if name(1:length(piece_strength_prefix)) == piece_strength_prefix
    plot_figure = piece_strength_figure;
    piece_scalar_index = 1;
  end
  
  if name(length(name) - length(scalar_suffix) + 1:end) == scalar_suffix
    plot_figure = scalar_figure;
    piece_scalar_index = 2;
  end

  conv_window = 100;
  if ~isnan(plot_figure) && length(this_data) > conv_window
    figure(plot_figure);
    hold all;
    smooth_data = conv(this_data, ones(conv_window, 1), 'valid')/conv_window;
    if piece_scalar_index == 1
      name = name(end);
    else
      name = name(1 : end - length(scalar_suffix));
    end
    plot(smooth_data, 'LineWidth', 3, 'displayname', name);
    piece_scalar_plots(piece_scalar_index) = true;
  end
end

if piece_scalar_plots(1)
  figure(piece_strength_figure);
  legend('show');
  print([gene_pool_filename '_piece_strength.png']);
end

if piece_scalar_plots(2)
  figure(scalar_figure);
  legend('show');
  print([gene_pool_filename '_gene_scalars.png']);
end
