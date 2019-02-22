#!/bin/bash

# Count how many times a castling move was picked (O-O = kingside, O-O-O = queenside)

bindir="$(dirname "$0")"
(echo "# Castling" ; grep O "$1" | ./"$bindir"/delete_comments.sh | tr -s '[:space:]' '[\n*]' | grep O | tr -d +\#  | sort | uniq -c)
echo "out of $(grep -c Round "$1") games"
