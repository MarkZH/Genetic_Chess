#!/bin/python

import sys

def main(argv):
    current_gene = ''
    header_line = []
    with open(argv[1] + '_parsed.txt', 'w') as w:
        with open(argv[1]) as f:
            for line in f:
                line = line.strip()
                if line == 'END':
                    w.write(','.join(header_line) + '\n')
                    break

                if ':' in line:
                    parameter, value = line.split(':')
                    if parameter == 'ID':
                        header_line.append(parameter)
                    elif parameter == 'Name':
                        current_gene = value.strip()
                    elif parameter == 'Still Alive':
                        continue
                    else:
                        header_line.append(current_gene + ' - ' + parameter)


        data_line = []
        with open(argv[1]) as f:
            parameter_count = 0
            for line in f:
                line = line.strip()
                if line == 'END':
                    w.write(','.join(data_line) + '\n')
                    data_line = []
                    parameter_count = 0
                    continue

                if ':' in line:
                    parameter, value = line.split(':')
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
