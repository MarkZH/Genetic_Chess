clear;
close all;

[filename, directory, ~] = uigetfile();
gene_pool_filename = fullfile(directory, filename);

if length(gene_pool_filename) == 0
  return
end

python('gene_pool_analyze.py', gene_pool_filename);
filename = [gene_pool_filename '_parsed.txt'];
data = importdata(filename, ',');
id_list = data.data(:, 1);
id_list_sorted = sort(id_list);

offset = -1;
missing_ids = [];
for index = 1 : length(id_list_sorted)
  while id_list_sorted(index) ~= index + offset
    missing_ids = [missing_ids ; index + offset];
    offset = offset + 1;
  end
end

for yi = 2 : length(data.colheaders)
  figure;
  hold all;
  high = max(data.data(:, yi));
  scatter(id_list, data.data(:, yi));
  if length(missing_ids) > 0
    plot(missing_ids, high*1.1*ones(size(missing_ids)), 'kx');
  end
  xlabel(data.colheaders(1){1});
  ylabel(data.colheaders(yi){1})
end