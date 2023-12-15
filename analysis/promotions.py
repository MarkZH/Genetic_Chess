#!/usr/bin/python

from collections import Counter
from common import game_moves

# Count how many times each type of piece is picked for a pawn promotion
def count_promotions(game_file_name: str) -> None:
    print("\n# Promotions")
    promotion_counts: Counter = Counter()
    with open(game_file_name) as game_file:
        while True:
            moves = game_moves(game_file)
            if not moves:
                break
            for move in moves:
                if "=" in move:
                    promotion = move.split("=")[1][0]
                    promotion_counts[promotion] += 1

    count_column_width = len(str(max(promotion_counts.values())))
    for piece, count in promotion_counts.most_common():
        spaces = ' '*(count_column_width - len(str(count)))
        print(spaces, count, piece)
