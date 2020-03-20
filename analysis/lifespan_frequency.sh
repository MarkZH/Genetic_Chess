#!/usr/bin/bash

# Every game played results in a child AI, so the number of games
# played by an AI equals the number of its offspring.

input_file="$1"
output_file="${input_file}_offspring_counts.txt"
excluded_id="$2"
grep "Genetic" "$input_file" |  # Find players
    cut -d\" -f2 |     # Cut out name of player
    cut -d' ' -f3 > "$output_file" # Cut out ID number

octave ./analysis/lifespan_plots.m "$output_file"
