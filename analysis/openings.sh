#!/bin/bash

# Determine most often used openings

game_file="$1"
bindir="$(dirname "$0")"
outfile="${1}_opening_list.txt"
echo "# Most popular openings:"
grep '^1\.' "$game_file"                | # First n moves
    ./"$bindir"/delete_comments.sh      |
    grep -v -- --                       | # Delete dashes between games
    sed -e 's/1-0//' -e 's/0-1//' -e 's/1\/2-1\/2//' | # delete endings
    sed 's/\s*$//g'                     | # Delete trailing whitespace
    cut -d' ' -f2,3                     |
    tee "$outfile"                      |
    sort                                |
    uniq -c                             |
    sort -n                             |
    tee >(echo $(wc -l) different openings)

cut -d' ' -f1 "$outfile" > "${outfile}_white.txt"
cut -d' ' -f2 "$outfile" > "${outfile}_black.txt"
