#!/bin/bash

# Count how many times each type of piece is picked for a pawn promotion

bindir="$(dirname "$0")"
echo "# Promotions"
grep = "$1" | ./"$bindir"/delete_comments.sh | tr -s '[:space:]' '[\n*]' | grep = | cut -d= -f2 | tr -d +\#  | sort | uniq -c

