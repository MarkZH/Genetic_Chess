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
  still_alive_gene = 0;
  for pool_id = 0 : max(pool_ids)
    id_right = (pool_ids == pool_id);
    plot(id_list(id_right), this_data(id_right), ...
         '.', ...
         'markersize', 10, ...
         'displayname', ['Pool ' num2str(pool_id)]);
    h = plot(id_list(still_alive & id_right), this_data(still_alive & id_right), ...
         'ok', ...
         'markersize', 10, ...
         'linewidth', 1);
    still_alive_gene = still_alive_gene + sum(isfinite(this_data(still_alive & id_right)));
    if pool_id == max(pool_ids)
      number_alive = num2str(still_alive_gene);
      set(h, 'displayname', ['Still Alive (' number_alive ')']);
    end
  end
  leg = legend('show');
  set(leg, 'location', 'southoutside');
  set(leg, 'orientation', 'horizontal');
  legend left;
  xlabel(xaxis, 'FontSize', 18);
  title(name, 'FontSize', 22);
  set(gca, 'FontSize', 14);
  print([gene_pool_filename '_gene_' name '.png']);
  
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
    conv_margin = floor(conv_window/2);
    x_axis = conv_margin : length(smooth_data) + conv_margin - 1;
    plot(x_axis, smooth_data, 'LineWidth', 3, 'displayname', name);
    piece_scalar_plots(piece_scalar_index) = true;
  end
end

if piece_scalar_plots(1)
  figure(piece_strength_figure);
  leg = legend('show');
  set(leg, 'orientation', 'horizontal');
  set(leg, 'location', 'southoutside');
  legend left;
  print([gene_pool_filename '_piece_strength.png']);
end

if piece_scalar_plots(2)
  figure(scalar_figure);
  leg = legend('show');
  set(leg, 'orientation', 'horizontal');
  set(leg, 'location', 'southoutside');
  legend left;
  print([gene_pool_filename '_gene_scalars.png']);
end
