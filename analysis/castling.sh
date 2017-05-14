#!/bin/bash

# Count how many times each type of piece is picked for a pawn promotion

bindir="$(dirname "$0")"
(echo "# Castling" ; grep O "$1" | ./"$bindir"/delete_comments.sh | tr -s '[:space:]' '[\n*]' | grep O | tr -d +\#  | sort | uniq -c)

