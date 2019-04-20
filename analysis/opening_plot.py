#!/usr/bin/python

import sys

def parse_opening_list(filename):
    with open(filename) as f:
        open_count = dict()
        openings = []
        for line in (raw.strip() for raw in f):
            open_count.setdefault(line, 0)
            open_count[line] += 1
            openings.append(line)

    top10 = list(reversed(sorted(open_count.keys(),
                            key=lambda x: open_count[x])[-10:]))

    movsum_window = 1000
    last_window = openings[-movsum_window:]
    top10_rate = list(reversed(sorted(open_count.keys(),
                                    key=lambda x : last_window.count(x))[-10:]))

    for data in [[top10, '_top_opening_data.txt'], [top10_rate, '_top_opening_rate_data.txt']]:
        with open(filename + data[1] , 'w') as out:
            out.write(','.join(data[0]) + '\n')
            for opening in openings:
                marker = ['1' if x == opening else '0' for x in data[0]]
                out.write(','.join(marker) + '\n')


if __name__ == '__main__':
    parse_opening_list(sys.argv[1])
