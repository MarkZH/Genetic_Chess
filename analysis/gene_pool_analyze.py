#!/bin/python

import sys

def main(argv):
    current_gene = ''
    data_line = []
    with open(argv[1] + '_parsed.txt', 'w') as w:
        with open(argv[1]) as f:
            for line in f:
                line = line.strip()
                if line == 'END':
                    w.write(','.join(data_line) + '\n')
                    break

                if ':' in line:
                    parameter, value = line.split(':')
                    if parameter == 'ID':
                        data_line.append(parameter)
                    elif parameter == 'Name':
                        current_gene = value.strip()
                    elif parameter == 'Still Alive':
                        continue
                    else:
                        data_line.append(current_gene + ' - ' + parameter)

        data_line = []
        with open(argv[1]) as f:
            for line in f:
                line = line.strip()
                if line == 'END':
                    w.write(','.join(data_line) + '\n')
                    data_line = []
                    continue

                if ':' in line:
                    parameter, value = line.split(':')
                    if parameter not in ['Name', 'Still Alive']:
                        data_line.append(value.strip())


if __name__ == '__main__':
    main(sys.argv)
