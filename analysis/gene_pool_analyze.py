#!/bin/python

import sys

def main(argv):
    current_gene = ''
    header_line = []
    ids_at_end = []
    still_alive = dict()
    pool = dict()
    header_written = False
    largest_pool_id = -1
    with open(argv[1] + '_parsed.txt', 'w') as w:
        with open(argv[1]) as f:
            for line in f:
                line = line.strip()
                if line == 'END' and not header_written:
                    w.write(','.join(header_line + ['Still Alive', 'Original Pool']) + '\n')
                    header_written = True

                if ':' in line:
                    parameter, value = line.split(':', 1)
                    if parameter == 'ID':
                        if not header_written:
                            header_line.append(parameter)
                        ids_at_end.append(value.strip())
                    elif parameter == 'Name':
                        current_gene = value.strip()
                    elif parameter == 'Still Alive':
                        ids_at_end = []
                        pool_id, ids = value.split(' : ')
                        pool_id = pool_id.strip()
                        if int(pool_id) > largest_pool_id:
                            largest_pool_id = int(pool_id)
                        last_pool_id = pool_id
                        ids = ids.split()
                        still_alive[pool_id] = ids
                        for ident in ids:
                            pool[ident] = pool_id
                    else:
                        header_line.append(current_gene + ' - ' + parameter)


        data_line = []
        still_alive_ids = []
        for ids in still_alive.values():
            still_alive_ids += ids

        with open(argv[1]) as f:
            parameter_count = 0
            for line in f:
                line = line.strip()
                if line == 'END':
                    is_still_alive = str(int(data_line[0] in still_alive_ids))
                    data_line.append(is_still_alive)

                    try:
                        pool_id = pool[data_line[0]]
                    except KeyError:
                        break # passed last Still Alive

                    data_line.append(pool_id)

                    w.write(','.join(data_line) + '\n')
                    data_line = []
                    parameter_count = 0
                    continue

                if ':' in line:
                    parameter, value = line.split(':', 1)
                    if parameter not in ['Name', 'Still Alive']:
                        data_line.append(value.strip())
                        parameter_count += 1
                    else:
                        parameter_count = 0
                elif line.strip() == 'INACTIVE':
                    for index in range(-parameter_count, 0):
                        data_line[index] = 'nan'


if __name__ == '__main__':
    main(sys.argv)
