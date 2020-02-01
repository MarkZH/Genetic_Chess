# Genetic Chess

An amateur attempt at breeding a chess-playing AI.

This program attempts to create a chess-playing AI by playing prospective AIs
against each and letting the winners produce mutated copies. As evidence that
this has a chance of working, here's a plot from a recent run of the value that
the AIs place on each type of piece.

![An example of the evolution of piece values](doc/pawn-crash-strength-plot.png)

It took awhile (tens of thousands of generations), but it did figure out that
the queen was an important piece. The vertical axis has arbitrary units that
indicate the relative value of each piece type.

See the `doc/` folder for an in-depth description of the running and working of
this project.

### Quick build instructions
#### Linux
0. Install `gcc` or `clang`
1. Clone this repository to your computer.
2. `./create_Makefile gcc` (or `clang`)
3. `make release`

#### Windows
1. Clone this repository to your computer.
2. Open Genetic_Chess.sln in Visual Studio and compile.

#### More documentation
- `make user_manual`, or
- Compile `doc/reference.tex` with TexWriter or similar.

### Start a game
Install a chess GUI (e.g., [PyChess](http://www.pychess.org/)): `$cwd/bin/release/genetic_chess -genetic genetic_ai_example.txt`

### Start a gene pool
1. Edit `gene_pool_config_example.txt` to adjust the number of simultaneous
   games to a number less than or equal to the number of processors on your
   computer.
2. `<bin location>/genetic_chess -genepool gene_pool_config_example.txt`
    * If compiling with Visual Studio, look in x86/Release or x64/Release
      for the compiled program.
3. Every few hours or days, pause the gene pool with `Ctrl-Z` (Linux) or `Ctrl-C`
   (on Windows) and run `analysis/run_all_plots.sh gene_pool_config_example.txt 1`
   to generate plots and other data showing the evolution and behavior of the specimens.
   * This script requires the program `octave` to run.
