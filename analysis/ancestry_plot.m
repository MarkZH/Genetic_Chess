isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;

filename = 0;
directory = '';
if isOctave
    graphics_toolkit('gnuplot');
    args = argv();
    if length(args) > 0
        filename = args{1};
    end
end

if filename == 0
    [filename, directory, ~] = uigetfile();
end

if filename == 0
    return
end
gene_pool_filename = fullfile(directory, filename);
raw_data_file_name = [gene_pool_filename '_ancestry.txt'];

if isOctave
    python('analysis/ancestry_parse.py', ['"' gene_pool_filename '"']);
end

data = importdata(raw_data_file_name);

id = data(:, 1);
origins = data(:, 2:end);

figure;
hold all;
for column = 1 : size(origins, 2)
    plot(id, origins(:, column), ...
         '.',
         'LineWidth', 3, ...
         'displayname', ['Pool ' num2str(column - 1)]);
end

xlabel('ID');
ylabel('Ancestry fraction');
title('Ancestral makeup');
leg = legend('show');
set(leg, 'Location', 'SouthOutside');
set(leg, 'Orientation', 'Horizontal');
legend left;

print([raw_data_file_name '_plot.png']);
