#!/bin/bash

# Count how many times each type of piece is picked for a pawn promotion

bindir="$(dirname "$0")"
#  First moves   |                                | sort and count unique lines
#                |                                | and sort by popularity
grep '^1\.' "$1" | ./"$bindir"/delete_comments.sh | sort | uniq -c | sort -n
