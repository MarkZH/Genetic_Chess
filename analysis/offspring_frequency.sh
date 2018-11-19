#!/usr/bin/bash

# Every game played results in a child AI, so the number of games
# played by an AI equals the number of its offspring.

input_file="$1"
output_file="${input_file}_offspring_counts.txt"
grep "\[" "$input_file" |           # Find header lines
    grep "White\|Black" |  # Find players
    cut -d\" -f2 |         # Cut out name of player
    cut -d' ' -f3 |        # Cut out ID number
    sort -n |              # Sort by number, then
    uniq -c |              # count appearances of ID (equals number of children)
    sed "s/^ *//" |        # Delete leading whitespace
    cut -d" " -f1 |        # Get list of number of children by each AI
    sort -n |              # Sort that list to
    uniq -c |              # count number of time an AI has sired N children
    column -t -o\; -O2,1 -N"Frequency,Child count" |
    tee "$output_file"

octave ./analysis/offspring_plots.m "$output_file"
