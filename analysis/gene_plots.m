isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
  graphics_toolkit('gnuplot'); % Octave only
  args = argv();
  if length(args) > 0
    filename = args{1};
    id_marks = [];
    if length(args) > 1
      marks_file_name = args{2};
      if ~isempty(marks_file_name)
        id_marks = importdata(marks_file_name)';
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

for yi = 2 : length(data.colheaders) - 2
  this_data = data.data(:, yi);
  name_list = data.colheaders(yi);
  name = name_list{1};

  figure('Position', [0, 0, 1200, 1000]);
  hold all;
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
    special_plot = true;
  end

  if strncmp(fliplr(name), fliplr(priority_suffix), length(priority_suffix))
    plot_figure = priority_figure;
    piece_priority_index = 2;
    special_plot = true;
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
