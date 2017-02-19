#!/bin/bash

# Count how many times each type of piece is picked for a pawn promotion

bindir="$(dirname "$0")"
#  First moves   |                                | Save list to file            | sort and count unique lines
#                |                                | for other uses               | and sort by popularity
grep '^1\.' "$1" | ./"$bindir"/delete_comments.sh | tee "${1}_opening_list.txt" | sort | uniq -c | sort -n
