import subprocess, os, sys

master = './Genetic_Chess_master'
new_king = './Genetic_Chess_new_king_check'
game_file = 'game.pgn'
count = 0

while True:
    for generator in [master, new_king]:
        os.remove(game_file)
        count += 1
        print('Game #' + str(count))
        if generator == master:
            checker = new_king
        else:
            checker = master
        subprocess.run([generator, '-random', '-random'])
        result = subprocess.run([checker, '-confirm', game_file])
        if result.returncode != 0:
            print('Found discrepancy. See ' + game_file)
            print('generator = ' + generator)
            print('checker = ' + checker)
            sys.exit()
