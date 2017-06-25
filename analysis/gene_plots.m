isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
  graphics_toolkit('gnuplot'); % Octave only
  args = argv();
  if length(args) > 0
    filename = args{1};
    id_marks = str2double(args(2:end))';
  end
end

if filename == 0
  [filename, directory, ~] = uigetfile();
end

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
xlabel('ID');

priority_figure = figure('Position', [0, 0, 1200, 1000]);
priority_suffix = ' Gene - Priority';
priority_count = 0;
title('Gene Priority Evolution', 'FontSize', 22);
xlabel('ID');

piece_priority_plots = [0, 0];

total_force_index = 0;
total_force_prefix = "Total Force Gene";
opponent_pieces_targeted_index = 0;
opponent_pieces_prefix = "Opponent Pieces Targeted Gene";
piece_strength_index = 0;

for yi = 2 : length(data.colheaders) - 2
  this_data = data.data(:, yi);
  name_list = data.colheaders(yi);
  name = name_list{1};

  figure('Position', [0, 0, 1200, 1000]);
  hold all;
  still_alive_gene = 0;
  for pool_id = 0 : max(pool_ids)
    id_right = (pool_ids == pool_id);
    good = (still_alive & id_right);
    plot(id_list(id_right), this_data(id_right), ...
         '.', ...
         'markersize', 10, ...
         'displayname', ['Pool ' num2str(pool_id)]);
    h = plot(id_list(good), this_data(good), ...
             'ok', ...
             'markersize', 10, ...
             'linewidth', 1);
    still_alive_gene = still_alive_gene + sum(isfinite(this_data(good)));
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
  plot(xlim, [0 0], 'k'); % X-axis

  % Fill in nan gaps in data
  this_data(~isfinite(this_data)) = 0;
  conv_window = 100;
  smooth_data = conv(this_data, ones(conv_window, 1), 'valid')/conv_window;
  conv_margin = floor(conv_window/2);
  x_axis = id_list(conv_margin : end - conv_margin);
  plot(x_axis, smooth_data, 'k', 'LineWidth', 3, 'displayname', 'Average');

  for n = id_marks
    plot(n*[1 1], ylim);
  end

  print([gene_pool_filename '_gene_' name '.png']);

  special_plot = false;
  if strncmp(name, piece_strength_prefix, length(piece_strength_prefix))
    plot_figure = piece_strength_figure;
    piece_priority_index = 1;
    piece_strength_index = yi;
    special_plot = true;
  end

  if strncmp(fliplr(name), fliplr(priority_suffix), length(priority_suffix))
    plot_figure = priority_figure;
    piece_priority_index = 2;
    special_plot = true;
  end

  if strncmp(name, total_force_prefix, length(total_force_prefix))
    total_force_index = yi;
  end

  if strncmp(name, opponent_pieces_prefix, length(opponent_pieces_prefix))
    opponent_pieces_targeted_index = yi;
  end

  if special_plot && length(this_data) > conv_window;
    figure(plot_figure);
    hold all;

    if piece_priority_index == 1
      name = name(end);
      make_dashed = false;
    else
      name = name(1 : end - length(priority_suffix));
      priority_count = priority_count + 1;
      make_dashed = (priority_count > 7);
    end

    p = plot(x_axis, smooth_data, 'LineWidth', 3, 'displayname', name);
    if make_dashed
      set(p, 'LineStyle', ':');
    end
    piece_priority_plots(piece_priority_index) = plot_figure;
  end
end

file_name_suffixes = {'_piece_strength.png', '_gene_priorities.png'};
for index = 1 : 2
  if ~piece_priority_plots(index)
    continue;
  end

  figure(piece_priority_plots(index));
  leg = legend('show');
  set(leg, 'orientation', 'horizontal');
  set(leg, 'location', 'southoutside');
  legend left;
  plot(xlim, [0 0], 'k'); % X-axis

  for n = id_marks
    plot(n*[1 1], ylim);
  end

  print([gene_pool_filename file_name_suffixes{index}]);
end

if total_force_index > 0 && opponent_pieces_targeted_index > 0 && piece_strength_index > 0
  figure('Position', [0, 0, 1200, 1000]);
  hold all;
  total_force_active = isfinite(data.data(:, total_force_index));
  opponent_targeted_active = isfinite(data.data(:, opponent_pieces_targeted_index));
  piece_strength_active = isfinite(data.data(:, piece_strength_index));
  activity = (total_force_active | opponent_targeted_active) & piece_strength_active;
  smooth_activity = conv(activity, ones(conv_window, 1), 'valid')/conv_window;
  plot(100*smooth_activity, 'LineWidth', 3, 'displayname', 'Piece Strength');
  title('Piece Strength Gene in use', 'FontSize', 22);
  xlabel('ID', 'FontSize', 18);
  ylabel('Percentage with activity', 'FontSize', 18);
  ylim([0 100]);
  set(gca, 'FontSize', 14);

  total_force_gene_functional = total_force_active & piece_strength_active;
  smooth_force = conv(total_force_gene_functional, ones(conv_window, 1), 'valid')/conv_window;
  plot(100*smooth_force, 'LineWidth', 3, 'displayname', 'Total Force');

  opponent_gene_functional = opponent_targeted_active & piece_strength_active;
  smooth_opponent = conv(opponent_gene_functional, ones(conv_window, 1), 'valid')/conv_window;
  plot(100*smooth_opponent, 'LineWidth', 3, 'displayname', 'Opponent Pieces Targeted');

  leg = legend('show');
  set(leg, 'location', 'southoutside');
  set(leg, 'orientation', 'horizontal');
  set(leg, 'FontSize', 10);

  for n = id_marks
    plot(n*[1 1], ylim);
  end

  print([gene_pool_filename '_piece_strength_gene_active.png']);
end
