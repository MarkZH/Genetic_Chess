#!/usr/bin/python

import sys

def parse_opening_list(filename):
    with open(filename) as f:
        openings = [line.strip() for line in f]

    open_count = dict()
    for opening in openings:
        open_count.setdefault(opening, 0)
        open_count[opening] += 1

    top20 = list(reversed(sorted(open_count.keys(),
                            key=lambda x: open_count[x])[-20:]))

    parsed_file_name = filename + '_top_opening_data.txt'
    with open(parsed_file_name, 'w') as out:
        out.write(','.join(top20) + '\n')
        for opening in openings:
            marker = ['1' if x == opening else '0' for x in top20]
            out.write(','.join(marker) + '\n')

    return parsed_file_name


if __name__ == '__main__':
    parse_opening_list(sys.argv[1])
