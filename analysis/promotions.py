#!/usr/bin/python

from collections import Counter
from common import Game_Record, print_sorted_count_table


# Count how many times each type of piece is picked for a pawn promotion
def count_promotions(all_games: list[Game_Record]) -> None:
    print("\n# Promotions")
    promotion_counts: Counter = Counter()
    for game in all_games:
        promotion_counts.update(move[move.index("=") + 1] for move in game.moves if "=" in move)

    print_sorted_count_table(promotion_counts.items())
