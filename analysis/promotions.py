#!/usr/bin/python

from collections import Counter
from common import Game_Record


# Count how many times each type of piece is picked for a pawn promotion
def count_promotions(all_games: list[Game_Record]) -> None:
    print("\n# Promotions")
    promotion_counts: Counter = Counter()
    for game in all_games:
        for move in game.moves:
            if "=" in move:
                promotion = move.split("=")[1][0]
                promotion_counts[promotion] += 1

    count_column_width = len(str(max(promotion_counts.values())))
    for piece, count in promotion_counts.most_common():
        spaces = ' '*(count_column_width - len(str(count)))
        print(spaces, count, piece)
