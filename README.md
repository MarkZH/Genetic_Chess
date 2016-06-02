# Genetic Chess
An amateur attempt at breeding a chess-playing AI.

## To compile
`make`

This will create executables in a newly created `bin/` directory.

## To run

`genetic_chess -genepool [file_name]`
This will start up a gene pool with Genetic_AIs playing against each other--mating, killing, mutating--all that good Darwinian stuff. The optional file name parameter will cause the program to load a gene pool from a previous run. Every genome will be written to a file.

`genetic_chess (-human|-genetic|-random) (-human|-genetic|-random)`
Starts a local game played in the terminal with an ASCII art board. The first parameter is the white player, the second is black.
 - `-human` - a human player. Moves are specified in algebraic notation indicated the starting and ending square.
 - `-genetic` - a Genetic AI player. If a file name follows, load the genes from that file.
 - `-random` - an AI player that chooses moves randomly from all legal moves.

A barely functional implementation of the [Chess Engine Communication Protocol](https://www.gnu.org/software/xboard/engine-intf.html) allows for play through xboard and similar programs (PyChess, etc.). When used this way, arguments are ignored.


## Genes currently active in Genetic AI instances


### Board Evaluation Genes

#### Freedom To Move Gene
Counts the number of legal moves available in the current position.

#### King Confinement Gene
Counts the king's legal moves.

#### Opponent Pieces Targeted Gene
Sums the total strength (as determined by the Piece Strength Gene below) of
the opponent's pieces currently under attack.

#### Pawn Advancement Gene
Measures the progress of all pawns towards the opposite side of the board with
bonuses for promotion.

#### Sphere of Influence Gene
Counts the number of squares attacked by all pieces, weighted by the square's
proximity to the opponent's king and inversely weighted by the attacking
piece's strength. A weaker piece attacking a square is better since there's
less motivation to retreat if it is threatened.

#### Total Force Gene
Sums the strength (according to the Piece Strength Gene) of all the player's
pieces on the board.



### Regulatory Genes

#### Branch Pruning Gene
Specifies when to cut off analyzing a line of moves based on whether the
current board evaluation is too low compared to the real board state.

#### Last Minute Panic Gene
If the time left in the game is less than the amount specified here, then
look-ahead on all lines is cut off.

#### Look Ahead Gene
Determines how many positions to examine based on the time left.

#### Piece Strength Gene
Specifies the importance or strength of each differet type of chess piece.
