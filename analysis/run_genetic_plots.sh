#!/usr/bin/bash

get_config_value()
{
    sed 's/#.*//' "$1" | grep "$2" | cut -f2 -d= | sed -e 's/^\s*//' -e 's/\s*$//'
}

is_numeric()
{
    [ "$1" -eq "$1" ] > /dev/null 2>&1
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

if ! is_numeric "$opening_moves"
then
    echo "Invalid argument for openings analysis: $opening_moves"
    exit 1
fi

if [[ -n "$notes_file" ]] && ! [[ -f "$notes_file" ]]
then
    echo "Invalid notes file: $notes_file"
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

octave analysis/gene_plots.m "$pool_file" "$notes_file" &
octave analysis/win_lose_draw_plotting.m "$game_file" "$notes_file" &

seed_id=
seed="$(get_config_value "$config_file" seed)"
if [[ -n "$seed" ]]
then
    seed_file="$(cut -d'/' -f1 <<< "$seed")"
    seed_id="$(cut -d'/' -f2 <<< "$seed")"
    if ! is_numeric "$seed_id"
    then
        seed_id="$(grep ID: "$seed_file" | tail -n1 | cut -d' ' -f2)"
    fi
fi
./analysis/lifespan_frequency.sh "$game_file" "$seed_id" &

if ./analysis/openings.sh "$game_file" "$opening_moves"
then
    opening_file="${game_file}_opening_list.txt"
    octave analysis/opening_plotting.m "$opening_file" "$notes_file" &
fi

./analysis/promotions.sh "$game_file"
./analysis/castling.sh "$game_file"

wait
