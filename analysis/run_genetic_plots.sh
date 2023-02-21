#!/usr/bin/bash

get_config_value()
{
    sed 's/#.*//' "$1" | grep "$2" | cut -f2 -d= | sed -e 's/^\s*//' -e 's/\s*$//'
}

config_file="$1"

if [[ -z "$config_file" ]]
then
    echo "Argument: <gene pool config file>"
    exit 1
fi

if ! [[ -f "$config_file" ]]
then
    echo "Invalid gene pool configuration file: $config_file"
    exit 1
fi

pool_file="$(get_config_value "$config_file" file)"
if ! [[ -f "$pool_file" ]]
then
    echo "Could not find gene pool file: $pool_file"
    exit 1
fi

game_file="${pool_file}_games.pgn"
if ! [[ -f "$game_file" ]]
then
    echo "Could not find game file: $game_file"
    exit 1
fi

octave analysis/gene_plots.m "$pool_file" &
octave analysis/win_lose_draw_plotting.m "$game_file" &

if ./analysis/openings.sh "$game_file"
then
    opening_file="${game_file}_opening_list.txt"
    octave analysis/opening_plotting.m "${opening_file}" "First move counts" &
    octave analysis/opening_plotting.m "${opening_file}_white.txt" "White's first move counts" &
    octave analysis/opening_plotting.m "${opening_file}_black.txt" "Black's first move counts" &
fi

./analysis/promotions.sh "$game_file"
./analysis/castling.sh "$game_file"

wait
