#!/usr/bin/python

from collections import Counter
from common import read_all_games, print_sorted_count_table


# Count how many times each type of piece is picked for a pawn promotion
def count_promotions(game_file_name: str) -> None:
    promotion_counts: Counter = Counter()
    for game in read_all_games(game_file_name):
        promotion_counts.update(move[move.index("=") + 1] for move in game.moves if "=" in move)

    print("\n# Promotions")
    print_sorted_count_table(promotion_counts.items())
