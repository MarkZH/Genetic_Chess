#!/bin/bash

# Count how many times a castling move was picked (O-O = kingside, O-O-O = queenside)

count_castles()
{
    # $1 = PGN file name
    # $2 = Move separator after filtering extra spaces
    # $3 = Which move to use (2 = White, 3 = Black, 1 = Both after separating moves by newline)
    delete_comments="./$(dirname "$0")/delete_comments.sh"
    grep O "$1" \
		| "$delete_comments" \
		| tr -s ' ' "$2" \
		| cut -d' ' -f"$3" \
		| grep O \
		| tr -d +\# \
		| sed 's/.*/\0\nTotal/' \
		| sort \
		| uniq -c
}

echo "# Castling"
count_castles "$1" '\n' 1

echo "# White Castling"
count_castles "$1" ' ' 2

echo "# Black Castling"
count_castles "$1" ' ' 3

echo "out of $(grep -c Round "$1") games"
