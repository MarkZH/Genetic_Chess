#!/usr/bin/python

from collections import defaultdict
from typing import Dict, List
from delete_comments import delete_comments

# Determine most often used openings

def remove_text(line: str, excision: str) -> str:
    index = line.find(excision)
    if index != -1:
        line = line[:index] + line[index + len(excision):]
        return remove_text(line, excision)
    else:
        return ' '.join(line.split())

def get_opening_files(game_file: str) -> List[str]:
    outfile = f"{game_file}_opening_list.txt"
    white_file = outfile + "_white.txt"
    black_file = outfile + "_black.txt"
    unique_opening_counter: Dict[str, int] = defaultdict(lambda: 0)
    with open(game_file) as input, open(outfile, 'w') as output, open(white_file, 'w') as white_output, open(black_file, 'w') as black_output:
        for line in input:
            if not line.startswith('1. '):
                continue

            line = delete_comments(line)
            for ending in ['1-0', '0-1', '1/2-1/2']:
                line = remove_text(line, ending)
            try:
                _, white_move, black_move = line.split()
            except ValueError:
                continue
            opening = f"{white_move} {black_move}"
            output.write(f"{opening}\n")
            white_output.write(f"{white_move}\n")
            black_output.write(f"{black_move}\n")
            unique_opening_counter[opening] += 1

    count_column_width = len(str(max(unique_opening_counter.values())))
    totals = [(count, opening) for opening, count in unique_opening_counter.items()]
    print("\n# Most popular openings:")
    for count, opening in sorted(totals):
        spaces = ' '*(count_column_width - len(str(count)))
        print(spaces, count, opening)

    return [outfile, white_file, black_file]