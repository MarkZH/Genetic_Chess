#!/usr/bin/python

import sys
import concurrent.futures
import traceback
from gene_plots import plot_genome
from win_lose_draw_plotting import plot_endgames
from opening_plotting import plot_all_openings
from promotions import count_promotions
from castling import count_all_castles


def get_config_value(config_file: str, parameter: str):
    parameter = ' '.join(parameter.split())
    with open(config_file) as config:
        for line in config:
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            param, value = line.split('=', 1)
            param = ' '.join(param.split())
            if parameter == param:
                return value.strip()

    raise RuntimeError(f"Parameter {parameter} not found in {config_file}")


if __name__ == "__main__":
    try:
        config_file = sys.argv[1]
    except IndexError:
        print("Argument: <gene pool config file>")
        sys.exit(1)

    pool_file = get_config_value(config_file, "gene pool file")
    game_file = f"{pool_file}_games.pgn"

    with concurrent.futures.ProcessPoolExecutor() as executor:
        processes = [executor.submit(plot_genome, pool_file),
                     executor.submit(plot_endgames, game_file),
                     executor.submit(plot_all_openings, game_file),
                     executor.submit(count_promotions, game_file),
                     executor.submit(count_all_castles, game_file)
                     ]

        for process in processes:
            error = process.exception()
            if error:
                traceback.print_exception(error)
