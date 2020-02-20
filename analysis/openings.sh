#!/bin/bash

# Determine most often used openings

game_file="$1"
moves=${2:-1} # number of starting moves (default 1)
paste_dashes=
for _ in $(seq 1 "$moves")
do
    paste_dashes=$paste_dashes'- '
done

bindir="$(dirname "$0")"
echo "# Most popular openings:"
grep -A$((moves-1)) '^1\.' "$game_file" | # First n moves
    ./"$bindir"/delete_comments.sh      |
    grep -v -- --                       | # Delete dashes between games
    paste -d' ' $paste_dashes           | # Combine opening moves into one line
    cut -d' ' -f 1-3                    | # Only include move text
    tee "${1}_opening_list.txt"         |
    sort                                |
    uniq -c                             |
    sort -n                             |
    tee >(echo $(wc -l) different openings)
