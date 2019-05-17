#!/bin/python

import sys

def main(gene_pool_file_name):
    still_alive = dict()
    pool = dict()
    header_done = False

    # Read file for gene names and gene pool associations
    with open(gene_pool_file_name) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if ':' in line:
                parameter, value = line.split(':', 1)
                if parameter == 'ID' and not header_done:
                    header_line = [parameter]
                elif parameter == 'Name':
                    current_gene = value.strip()
                elif parameter == 'Still Alive':
                    pool_id, ids = value.split(':')
                    pool_id = pool_id.strip()
                    id_list = ids.split()
                    still_alive[pool_id] = id_list
                    for ident in id_list:
                        pool[ident] = pool_id
                elif not header_done:
                    header_line.append(current_gene + ' - ' + parameter)
            elif line == 'END':
                header_done = True

    # Read gene pool file for data
    output_file_name = gene_pool_file_name + '_parsed.txt'
    with open(gene_pool_file_name) as f, open(output_file_name, 'w') as w:
        data_line = []
        current_gene = ''
        w.write(','.join(header_line + ['Still Alive', 'Original Pool']) + '\n')
        for line in f:
            line = line.split('#')[0].strip()
            if not line:
                continue

            if line == 'END':
                try:
                    ident = data_line[0]
                    in_pool = pool[ident]
                    data_line.append(str(int(ident in still_alive[in_pool])))
                    data_line.append(in_pool)
                    w.write(','.join(data_line) + '\n')
                except KeyError:
                    # passed last Still Alive or somehow
                    # Pool ID wasn't recorded
                    pass

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
                    index = header_line.index(title)
                    while index >= len(data_line):
                        data_line.append('')
                    if data_line[index]:
                        raise Exception('Value already found: ' + title + ' for ID ' + str(data_line[0]))
                    data_line[index] = value


if __name__ == '__main__':
    main(sys.argv[1])
