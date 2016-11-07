#!/bin/python

import sys

def main(files):
    for file_name in files:
        game = 0
        white_wins = 0
        black_wins = 0
        draws = 0
        with open(file_name) as f, open(file_name + '_plots.txt', 'w') as w:
            w.write('\t'.join(['Game', \
                               'White Wins', \
                               'Black Wins', \
                               'Draws', \
                               'Time',
                               'White Time Left',
                               'Black Time Left']) + '\n')
            # result_type key:
            #   0 = Checkmate
            #   1 = 50-move
            #   2 = 3-fold repitition
            #   3 = Time forfeit
            #   4 = No legal moves
            time_section = False
            for line in f:
                if line.startswith('[Result'):
                    game += 1
                    result = line.split('"')[1]
                    if result == '1/2-1/2':
                        draws += 1
                    elif result == '1-0':
                        white_wins += 1
                        result_type = 0
                    else:
                        black_wins += 1
                        result_type = 0
                elif line.startswith('[Termination'):
                    result_text = line.split('"')[1]
                    if result_text.lower() in ['threefold repitition', 'threefold repetition']:
                        result_type = 2
                    elif result_text.lower() == '50-move limit':
                        result_type = 1
                    elif result_text.lower() == 'time forfeiture':
                        result_type = 3
                    else:
                        result_type = 4
                elif 'Initial time' in line:
                    time_section = True
                    time = int(line.split(':')[1].strip())
                elif time_section and 'White' in line:
                    white_time_left = line.split()[-1]
                elif time_section and 'Black' in line:
                    black_time_left = line.split()[-1]
                elif time_section and not line.strip():
                    w.write('\t'.join(str(x) for x in [game, white_wins, black_wins, draws, time, result_type, white_time_left, black_time_left]) + '\n')
                    time_section = False

if __name__ == '__main__':
    main(sys.argv[1:])
