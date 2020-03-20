#!/usr/bin/bash

input_file="$1"
output_file="${input_file}_games_survived_counts.txt"
excluded_id="$2"
grep "Genetic" "$input_file" |  # Find players
    cut -d\" -f2 |     # Cut out name of player
    cut -d' ' -f3 > "$output_file" # Cut out ID number

octave ./analysis/lifespan_plots.m "$output_file"
