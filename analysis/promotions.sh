#!/bin/bash

# Count how many times each type of piece is picked for a pawn promotion

# Delete {} comments    | put each move on own line | look for promotions | get promoted piece alone | sort and count
sed 's/{[^}]*}//g' "$1" | tr -s '[:space:]' '[\n*]' | grep =              | cut -d= -f2 | tr -d +\#  | sort | uniq -c

