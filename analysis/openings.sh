#!/bin/bash

# Count how many times each type of piece is picked for a pawn promotion

# First moves    |         Delete {} comments and whitespace            | sort and count unique lines and sort by popularity
grep '^1\.' "$1" | sed -e 's/{[^}]*}//g' -e 's/\s\+/ /g' -e 's/\s\+$//' | sort | uniq -c | sort -n
