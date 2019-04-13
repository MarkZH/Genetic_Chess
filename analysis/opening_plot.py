#!/usr/bin/python

import sys

def parse_opening_list(filename):
    with open(filename) as f:
        open_count = dict()
        openings = []
        for line in (raw.strip() for raw in f):
            try:
                open_count[line] += 1
            except KeyError:
                open_count[line] = 1
            openings.append(line)
        top10 = list(reversed(sorted(open_count.keys(),
                                key=lambda x: open_count[x])[-10:]))

    with open(filename + '_top_opening_data.txt', 'w') as out:
        out.write(','.join(top10) + '\n')
        for opening in openings:
            marker = ['0']*len(top10)
            if opening in top10:
                marker[top10.index(opening)] = '1'
            out.write(','.join(marker) + '\n')

    with open(filename + '_all_opening_data.txt', 'w') as out:
        unique_openings = list(open_count.keys())
        out.write(','.join(unique_openings) + '\n')
        for opening in openings:
            marker = ['0']*len(unique_openings)
            marker[unique_openings.index(opening)] = '1'
            out.write(','.join(marker) + '\n')

if __name__ == '__main__':
    parse_opening_list(sys.argv[1])
