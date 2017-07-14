#!/bin/python

import sys

def main(gene_pool_file_name):
    still_alive = dict()
    pool = dict()

    # Read file for gene names
    with open(gene_pool_file_name) as f:
        for line in f:
            line = line.strip()
            if ':' in line:
                parameter, value = line.split(':', 1)
                if parameter == 'ID':
                    header_line = [parameter]
                elif parameter == 'Name':
                    current_gene = value.strip()
                elif parameter == 'Still Alive':
                    pool_id, ids = value.split(' : ')
                    pool_id = pool_id.strip()
                    ids = ids.split()
                    still_alive[pool_id] = ids
                    for ident in ids:
                        pool[ident] = pool_id
                else:
                    header_line.append(current_gene + ' - ' + parameter)

        # Read gene pool file for data
        output_file_name = gene_pool_file_name + '_parsed.txt'
        with open(gene_pool_file_name) as f, open(output_file_name, 'w') as w:
            data_line = []
            current_gene = ''
            w.write(','.join(header_line + ['Still Alive', 'Original Pool']) + '\n')
            for line in f:
                line = line.strip()
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
                        pass
                    else:
                        if current_gene:
                            title = current_gene + ' - ' + parameter
                        else:
                            title = parameter
                        while not header_line[len(data_line)] == title:
                            data_line.append('0')
                        data_line.append(value)


if __name__ == '__main__':
    main(sys.argv[1])
