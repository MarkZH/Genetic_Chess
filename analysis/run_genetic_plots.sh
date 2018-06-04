#!/usr/bin/bash

pool_file="$1"
opening_moves="$2"
notes_file="$3"

if [[ -z "$pool_file" ]]
then
    echo "Specify a gene pool file to analyze."
    exit 1
fi

game_file="${pool_file}_games.txt"
opening_file="${game_file}_opening_list.txt"
octave analysis/gene_plots.m "$pool_file" "$notes_file" &
octave analysis/win_lose_draw_plotting.m "$game_file" "$notes_file" &
./analysis/openings.sh "$game_file" "$opening_moves" && octave analysis/opening_plotting.m "$opening_file" "$notes_file" &
wait
./analysis/promotions.sh "$game_file"
./analysis/castling.sh "$game_file"
