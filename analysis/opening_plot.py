#!/usr/bin/python

import sys

def parse_opening_list(filename):
    with open(filename) as f:
        openings = [line.strip() for line in f.readlines()]

    open_count = dict()
    for opening in openings:
        open_count.setdefault(opening, 0)
        open_count[opening] += 1

    top10 = list(reversed(sorted(open_count.keys(),
                            key=lambda x: open_count[x])[-10:]))

    with open(filename + '_top_opening_data.txt', 'w') as out:
        out.write(','.join(top10) + '\n')
        for opening in openings:
            marker = ['1' if x == opening else '0' for x in top10]
            out.write(','.join(marker) + '\n')


if __name__ == '__main__':
    parse_opening_list(sys.argv[1])
