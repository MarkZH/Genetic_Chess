#!/usr/bin/bash

get_config_value()
{
    sed 's/#.*//' "$1" | grep "$2" | cut -f2 -d= | sed -e 's/^\s*//' -e 's/\s*$//'
}

config_file="$1"
opening_moves="$2"
notes_file="$3"

if [[ -z "$config_file" ]]
then
    echo "Arguments: <gene pool config file> <moves in opening> <notes file>"
    echo "Only the first is required."
    exit 1
fi

if ! [[ -f "$config_file" ]]
then
    echo "Invalid gene pool configuration file: $config_file"
    exit 1
fi

if [[ -n "$notes_file" ]] && ! [[ -f "$notes_file" ]]
then
    echo "Invalid notes file: $notes_file"
    exit 1
fi

pool_file="$(get_config_value "$config_file" file)"
game_file="${pool_file}_games.pgn"
opening_file="${game_file}_opening_list.txt"
octave analysis/gene_plots.m "$pool_file" "$notes_file" &
octave analysis/win_lose_draw_plotting.m "$game_file" "$notes_file" &

# Second term here checks if argument is a number
if [[ -n "$opening_moves" ]] && [ "$opening_moves" -eq "$opening_moves" ]
then
    ./analysis/openings.sh "$game_file" "$opening_moves" && octave analysis/opening_plotting.m "$opening_file" "$notes_file" &
fi

wait # for opening_plotting.m to finish outputing to command line
./analysis/promotions.sh "$game_file"
./analysis/castling.sh "$game_file"

reproduction="$(get_config_value "$config_file" reproduction)"
./analysis/offspring_frequency.sh "$game_file" "$reproduction"
