#!/usr/bin/bash

# Every game played results in a child AI, so the number of games
# played by an AI equals the number of its offspring.

output_file="${1}_offspring_counts.txt"
reproduction_type="$2"
grep "Genetic" "$1" |  # Find players
    cut -d\" -f2 |     # Cut out name of player
    cut -d' ' -f3 > "$output_file" # Cut out ID number

octave ./analysis/offspring_plots.m "$output_file" "$reproduction_type"
