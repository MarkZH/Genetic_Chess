#!/bin/python

import sys

def main(gene_pool_file_name):
    # Read gene file for gene names
    header_line = []
    current_gene = ''
    with open(gene_pool_file_name) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            if ':' in line:
                parameter, value = line.split(':', 1)
                parameter = parameter.strip()
                if parameter == 'Still Alive':
                    continue
                elif parameter == 'ID':
                    header_line.append(parameter)
                elif parameter == 'Name':
                    current_gene = value.strip()
                else:
                    header_line.append(current_gene + ' - ' + parameter)
            elif line == 'END':
                break
            else:
                raise Exception('Unknown line format: ' + line)

    # Read gene pool file for data
    output_file_name = gene_pool_file_name + '_parsed.txt'
    with open(gene_pool_file_name) as f, open(output_file_name, 'w') as w:
        data_line = []
        current_gene = ''
        w.write(','.join(header_line) + '\n')
        for line in f:
            line = line.split('#')[0].strip()
            if not line:
                continue

            if line == 'END':
                w.write(','.join(data_line) + '\n')
                current_gene = ''
                data_line = []
            elif ':' in line:
                parameter, value = line.split(':', 1)
                value = value.strip()
                if parameter == 'Name':
                    current_gene = value
                elif parameter == 'Still Alive':
                    continue
                else:
                    if current_gene:
                        title = current_gene + ' - ' + parameter
                    else:
                        title = parameter #ID
                    if parameter == 'Search Method':
                        value = '0' if value == 'Minimax' else '1'
                    index = header_line.index(title)
                    while index >= len(data_line):
                        data_line.append('')
                    if data_line[index]:
                        raise Exception('Value already found: ' + title + ' for ID ' + str(data_line[0]))
                    data_line[index] = value


if __name__ == '__main__':
    main(sys.argv[1])
