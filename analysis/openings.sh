#!/bin/bash

# Determine most often used openings

game_file="$1"
moves=${2:-1} # number of starting moves (default 1)
paste_dashes=
for n in $(seq 1 $moves)
do
    paste_dashes=$paste_dashes'- '
done

bindir="$(dirname "$0")"
#  First n moves                |                                | Delete "--"   | Merge two moves           | Save list to file           | sort and count unique lines
#                               |                                | between games | to one line               | for other uses              | and sort by popularity
grep -A$((moves-1)) '^1\.' "$1" | ./"$bindir"/delete_comments.sh | grep -v -- -- | paste -d' ' $paste_dashes | tee "${1}_opening_list.txt" | sort | uniq -c | sort -n
