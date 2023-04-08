#!/usr/bin/python

from collections import defaultdict
from typing import Dict, List
from delete_comments import delete_comments

# Determine most often used openings

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
            parts = line.split()

            def get_part(index):
                return parts[index] if len(parts) > index and not parts[index][0].isdigit() else ""
            
            white_move = get_part(1)
            black_move = get_part(2)
            opening = f"{white_move} {black_move}".strip()
            if opening:
                output.write(f"{opening}\n")
                unique_opening_counter[opening] += 1
            if white_move:
                white_output.write(f"{white_move}\n")
            if black_move:
                black_output.write(f"{black_move}\n")

    count_column_width = len(str(max(unique_opening_counter.values())))
    totals = [(count, opening) for opening, count in unique_opening_counter.items()]
    print("\n# Most popular openings:")
    for count, opening in sorted(totals):
        spaces = ' '*(count_column_width - len(str(count)))
        print(spaces, count, opening)

    return [outfile, white_file, black_file]