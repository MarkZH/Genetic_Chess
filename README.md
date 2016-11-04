# Genetic Chess
An amateur attempt at breeding a chess-playing AI.

## To compile
`make`

This will create release and debug executables in a newly created `bin/` directory.

Other usual commands: `make debug`, `make release`, and `make clean`.

## To run

`genetic_chess -genepool [file_name]`
This will start up a gene pool with Genetic_AIs playing against each other--mating, killing, mutating--all that good Darwinian stuff. The required file name parameter will cause the program to load a gene pool and other settings from a configuration file. A record of every genome and game played will be written to text files.

`genetic_chess (-human|-genetic|-random) (-human|-genetic|-random)`
Starts a local game played in the terminal with an ASCII art board. The first parameter is the white player, the second is black.
 - `-human`   - a human player. Moves are specified in algebraic notation indicated the starting and ending square.
 - `-genetic` - a Genetic AI player. If a file name follows, load the genes from that file. If there are several genomes in a file, the file name can be followed by a number to load the genome with that ID.
 - `-random`  - an AI player that chooses moves randomly from all legal moves.

A barely functional implementation of the [Chess Engine Communication Protocol](https://www.gnu.org/software/xboard/engine-intf.html) allows for play through xboard and similar programs (PyChess, etc.). When used this way, arguments are ignored. Future feature: specify a specific AI to use like -genetic or -random.


## Genes currently active in Genetic AI instances


### Board Evaluation Genes

#### Freedom To Move Gene
Counts the number of legal moves available in the current position.

#### King Confinement Gene
Counts the king's legal moves.

#### King Protection Gene
Counts the squares that have access to the king by any valid piece movement that are unguarded by that king's other pieces.

#### Opponent Pieces Targeted Gene
Sums the total strength (as determined by the Piece Strength Gene below) of
the opponent's pieces currently under attack.

#### Pawn Advancement Gene
Measures the progress of all pawns towards the opposite side of the board.

#### Sphere of Influence Gene
Counts the number of squares attacked by all pieces. Bonus points are awarded
if the square can be attacked with a legal move.

#### Total Force Gene
Sums the strength (according to the Piece Strength Gene) of all the player's
pieces on the board.


### Regulatory Genes

#### Look Ahead Gene
Determines how many positions to examine based on the time left. When looking
ahead to future moves, the number of positions to examine is divided equally
amongst every legal move. This naturally limits the depth of search while
allowing deeper searches for positions with fewer legal moves. The amount of
time to use in examing moves is determined by genetic factors indicating an
average number of moves per game and the number of positions than can be
examined per second. The distribution of moves per game is modeled with a
Poisson distribution.

#### Piece Strength Gene
Specifies the importance or strength of each differet type of chess piece.
