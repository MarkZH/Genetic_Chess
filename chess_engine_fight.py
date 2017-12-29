import subprocess, os, sys

master = './Genetic_Chess_master'
new_king = './Genetic_Chess_new_king_check'
game_file = 'game.pgn'

while True:
    # Run master to generate game, check if new_king_check
    # rules any moves illegal
    for generator in [master, new_king]:
        os.remove(game_file)
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
