import subprocess, os, sys

if len(sys.argv) < 2:
    print('Must specify file names of 2 chess engines')
    for i in range(len(sys.argv)):
        print(str(i) + ': ' + sys.argv[i])
    sys.exit(1)

first_engine = './' + sys.argv[-2]
second_engine = './' + sys.argv[-1]
game_file = 'game.pgn'
count = 0

while True:
    for generator in [first_engine, second_engine]:
        try:
            os.remove(game_file)
        except OSError:
            pass

        count += 1
        print('Game #' + str(count))

        if generator == first_engine:
            checker = second_engine
        else:
            checker = first_engine

        out = subprocess.run([generator, '-random', '-random'])
        if not os.path.isfile(game_file):
            print('Game file not produced: ' + game_file)
            print('generator = ' + generator)
            print(out.returncode)
            print(out.stdout)
            print(out.stderr)
            sys.exit()

        result = subprocess.run([checker, '-confirm', game_file])
        if result.returncode != 0:
            print('Found discrepancy. See ' + game_file)
            print('generator = ' + generator)
            print('checker = ' + checker)
            sys.exit()
