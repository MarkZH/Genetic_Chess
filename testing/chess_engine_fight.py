#!/usr/bin/python

import subprocess, os, sys

generator = './' + sys.argv[1]
checker = './' + sys.argv[2]
board = '' if len(sys.argv) == 3 else sys.argv[3]

game_file = 'engine_fight.pgn'
count = 0

while True:
    try:
        os.remove(game_file)
    except OSError:
        pass

    if os.path.isfile(game_file):
        print('Could not delete output file:', game_file)

    count += 1
    print('Game #' + str(count))

    if board:
        out = subprocess.run([generator, '-random', '-random', '-game_file', game_file, '-board', board])
    else:
        out = subprocess.run([generator, '-random', '-random', '-game_file', game_file])

    if not os.path.isfile(game_file):
        print('Game file not produced: ' + game_file)
        print('generator = ' + generator)
        print(out.returncode)
        print(out.stdout)
        print(out.stderr)
        break

    result = subprocess.run([checker, '-confirm', game_file])
    if result.returncode != 0:
        print('Found discrepancy. See ' + game_file)
        print('generator = ' + generator)
        print('checker = ' + checker)
        break

    generator, checker = checker, generator
