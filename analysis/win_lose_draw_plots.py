#!/bin/python

import sys

def main(files):
    for file_name in files:
        game = 0
        white_wins = 0
        black_wins = 0
        draws = 0
        number_of_moves = 0
        time = 0
        white_time_left = 0
        black_time_left = 0
        with open(file_name) as f, open(file_name + '_plots.txt', 'w') as w:
            w.write('\t'.join(['Game',
                               'White Wins',
                               'Black Wins',
                               'Draws',
                               'Result TYpe',
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
            #   8 = Time expired with insufficient material
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
                    if result_text.lower() == 'threefold repetition':
                        result_type = 3
                    elif result_text.lower() == '50-move limit':
                        result_type = 2
                    elif result_text.lower() == 'time forfeiture':
                        result_type += 4
                    elif result_text.lower() == 'insufficient material':
                        result_type = 6
                    elif result_text.lower() == 'stalemate':
                        result_type = 7
                    elif result_text.lower() == 'time expired with insufficient material':
                        result_type = 8
                    else:
                        raise Exception('Unrecognized result type: ' + result_text)
                elif line and line[0] in '123456789':
                    game_section = True
                    number_of_moves = line.split('. ')[0]
                elif line.startswith('[TimeControl'):
                    time = line.split('"')[1]
                elif line.startswith('[TimeLeftWhite'):
                    white_time_left = line.split('"')[1]
                elif line.startswith('[TimeLeftBlack'):
                    black_time_left = line.split('"')[1]
                elif line.startswith('[Event') and game > 0:
                    w.write('\t'.join(str(x) for x in [game,
                                                       white_wins,
                                                       black_wins,
                                                       draws,
                                                       time,
                                                       result_type,
                                                       white_time_left,
                                                       black_time_left,
                                                       number_of_moves]) + '\n')
                    number_of_moves = 0

            if game > 0:
                w.write('\t'.join(str(x) for x in [game,
                                                   white_wins,
                                                   black_wins,
                                                   draws,
                                                   time,
                                                   result_type,
                                                   white_time_left,
                                                   black_time_left,
                                                   number_of_moves]) + '\n')

if __name__ == '__main__':
    main(sys.argv[1:])
