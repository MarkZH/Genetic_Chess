#!/usr/bin/python

import sys
from collections import Counter
from delete_comments import delete_comments

# Count how many times each type of piece is picked for a pawn promotion

def count_promotions(game_file_name: str):
    print("\n# Promotions")
    promotion_counts = Counter()
    with open(game_file_name) as game_file:
        for line in game_file:
            if "=" not in line:
                continue
            line = delete_comments(line)
            for scrap in line.split():
                if "=" not in scrap:
                    continue
                promotion = scrap.split("=")[1].replace("+", "").replace("#",  "")
                promotion_counts[promotion] += 1

    count_column_width = len(str(max(promotion_counts.values())))
    for piece, count in promotion_counts.most_common():
        spaces = ' '*(count_column_width - len(str(count)))
        print(spaces, count, piece)


if __name__ == "__main__":
    count_promotions(sys.argv[1])