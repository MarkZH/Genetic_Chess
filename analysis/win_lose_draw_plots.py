#!/bin/python

import sys

def main(files):
    for file_name in files:
        game = 0
        white_wins = 0
        black_wins = 0
        draws = 0
        number_of_moves = 0
        with open(file_name) as f, open(file_name + '_plots.txt', 'w') as w:
            w.write('\t'.join(['Game', \
                               'White Wins', \
                               'Black Wins', \
                               'Draws', \
                               'Time',
                               'White Time Left',
                               'Black Time Left',
                               'Number of Moves']) + '\n')
            # result_type key:
            #   0 = Checkmate (white win)
            #   1 = Checkmate (black win)
            #   2 = 50-move
            #   3 = 3-fold repitition
            #   4 = Time forfeit (white win)
            #   5 = Time forfeit (black win)
            #   6 = Insufficient material
            #   7 = No legal moves
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
                        result_type = 1
                elif line.startswith('[Termination'):
                    result_text = line.split('"')[1]
                    if result_text.lower() in ['threefold repitition', 'threefold repetition']:
                        result_type = 3
                    elif result_text.lower() == '50-move limit':
                        result_type = 2
                    elif result_text.lower() == 'time forfeiture':
                        result_type += 4
                    elif result_text.lower() == 'insufficient material':
                        result_type = 6
                    else: # Stalemate
                        result_type = 7
                elif '. ' in line:
                    number_of_moves = line.split('. ')[0]
                elif 'Initial time' in line:
                    time_section = True
                    time = line.split(':')[1].split()[0].strip()
                elif time_section and 'White' in line:
                    white_time_left = line.split()[-2]
                elif time_section and 'Black' in line:
                    black_time_left = line.split()[-2]
                elif time_section and not line.strip():
                    w.write('\t'.join(str(x) for x in [game,
                                                       white_wins,
                                                       black_wins,
                                                       draws,
                                                       time,
                                                       result_type,
                                                       white_time_left,
                                                       black_time_left,
                                                       number_of_moves]) + '\n')
                    time_section = False
                    number_of_moves = 0

if __name__ == '__main__':
    main(sys.argv[1:])
