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
                elif parameter == 'Sorter Order':
                    sorters = [name.strip() for name in value.split(',')]
                    for sorter in sorters:
                        header_line.append(current_gene + " - " + parameter + " - " + sorter)
                else:
                    header_line.append(current_gene + ' - ' + parameter)
            elif line == 'END':
                break
            else:
                raise Exception('Unknown line format: ' + line)

    # Read gene pool file for data
    output_file_name = gene_pool_file_name + '_parsed.txt'
    with open(gene_pool_file_name) as f, open(output_file_name, 'w') as w:
        new_data_line = ['']*len(header_line)
        data_line = new_data_line.copy()
        current_gene = ''
        w.write(','.join(header_line) + '\n')
        for line in f:
            line = line.split('#')[0].strip()
            if not line:
                continue

            if line == 'END':
                data_line = [x or '0' for x in data_line]
                w.write(','.join(data_line) + '\n')
                current_gene = ''
                data_line = new_data_line.copy()
            elif ':' in line:
                parameter, value = line.split(':', 1)
                value = value.strip()
                if parameter == 'Name':
                    current_gene = value
                elif parameter == 'Still Alive':
                    continue
                else:
                    if current_gene:
                        if parameter == "Sorter Order":
                            first_order = value.split(',')[0].strip()
                            title = current_gene + ' - ' + parameter + ' - ' + first_order
                            value = '1'
                        else:
                            title = current_gene + ' - ' + parameter
                    else:
                        title = parameter #ID
                    index = header_line.index(title)
                    if data_line[index]:
                        raise Exception('Value already found: ' + title + ' for ID ' + str(data_line[0]))
                    data_line[index] = value


if __name__ == '__main__':
    main(sys.argv[1])
