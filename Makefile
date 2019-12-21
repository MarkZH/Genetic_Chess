BIN = genetic_chess
CXX = g++
LD = g++

CFLAGS = -fno-rtti -Iinclude -lstdc++fs -pedantic -std=c++17 -Wall -Wcast-align -Werror -Wextra -Wfloat-equal -Wmain -Wshadow -Wundef -Wunreachable-code -Wzero-as-null-pointer-constant
LDFLAGS = -pthread

DOC_DIR = doc/doxygen/html
DOC_INDEX = $(DOC_DIR)/index.html
ALL_SOURCES = main.cpp src/Testing.cpp include/Testing.h src/Players/Game_Tree_Node_Result.cpp include/Players/Game_Tree_Node_Result.h src/Players/UCI_Mediator.cpp include/Players/UCI_Mediator.h src/Players/Outside_Communicator.cpp include/Players/Outside_Communicator.h src/Players/CECP_Mediator.cpp include/Players/CECP_Mediator.h src/Players/Minimax_AI.cpp include/Players/Minimax_AI.h src/Players/Player.cpp include/Players/Player.h src/Players/Random_AI.cpp include/Players/Random_AI.h src/Players/Genetic_AI.cpp include/Players/Genetic_AI.h src/Breeding/Gene_Pool.cpp include/Breeding/Gene_Pool.h src/Exceptions/Game_Ended.cpp include/Exceptions/Game_Ended.h src/Exceptions/Bad_Still_Alive_Line.cpp include/Exceptions/Bad_Still_Alive_Line.h src/Utility/Random.cpp include/Utility/Random.h src/Utility/Scoped_Stopwatch.cpp include/Utility/Scoped_Stopwatch.h src/Utility/Configuration.cpp include/Utility/Configuration.h src/Utility/Math.cpp include/Utility/Math.h src/Utility/String.cpp include/Utility/String.h src/Game/Game.cpp include/Game/Game.h src/Game/Piece.cpp include/Game/Piece.h src/Game/Square.cpp include/Game/Square.h src/Game/Board.cpp include/Game/Board.h src/Game/Game_Result.cpp include/Game/Game_Result.h src/Game/Color.cpp include/Game/Color.h src/Game/Clock.cpp include/Game/Clock.h src/Genes/Pawn_Advancement_Gene.cpp include/Genes/Pawn_Advancement_Gene.h src/Genes/King_Confinement_Gene.cpp include/Genes/King_Confinement_Gene.h src/Genes/Genome.cpp include/Genes/Genome.h src/Genes/Piece_Strength_Gene.cpp include/Genes/Piece_Strength_Gene.h src/Genes/Null_Gene.cpp include/Genes/Null_Gene.h src/Genes/Freedom_To_Move_Gene.cpp include/Genes/Freedom_To_Move_Gene.h src/Genes/Stacked_Pawns_Gene.cpp include/Genes/Stacked_Pawns_Gene.h src/Genes/Sphere_of_Influence_Gene.cpp include/Genes/Sphere_of_Influence_Gene.h src/Genes/Total_Force_Gene.cpp include/Genes/Total_Force_Gene.h src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Genes/Opponent_Pieces_Targeted_Gene.h src/Genes/King_Protection_Gene.cpp include/Genes/King_Protection_Gene.h src/Genes/Checkmate_Material_Gene.cpp include/Genes/Checkmate_Material_Gene.h src/Genes/Mutation_Rate_Gene.cpp include/Genes/Mutation_Rate_Gene.h src/Genes/Pawn_Islands_Gene.cpp include/Genes/Pawn_Islands_Gene.h src/Genes/Gene.cpp include/Genes/Gene.h src/Genes/Castling_Possible_Gene.cpp include/Genes/Castling_Possible_Gene.h src/Genes/Look_Ahead_Gene.cpp include/Genes/Look_Ahead_Gene.h src/Genes/Passed_Pawn_Gene.cpp include/Genes/Passed_Pawn_Gene.h src/Moves/Pawn_Capture.cpp include/Moves/Pawn_Capture.h src/Moves/Pawn_Promotion_by_Capture.cpp include/Moves/Pawn_Promotion_by_Capture.h src/Moves/Pawn_Promotion.cpp include/Moves/Pawn_Promotion.h src/Moves/Move.cpp include/Moves/Move.h src/Moves/Castle.cpp include/Moves/Castle.h src/Moves/Pawn_Move.cpp include/Moves/Pawn_Move.h src/Moves/En_Passant.cpp include/Moves/En_Passant.h src/Moves/Pawn_Double_Move.cpp include/Moves/Pawn_Double_Move.h main.cpp src/Testing.cpp include/Testing.h src/Players/Game_Tree_Node_Result.cpp include/Players/Game_Tree_Node_Result.h src/Players/UCI_Mediator.cpp include/Players/UCI_Mediator.h src/Players/Outside_Communicator.cpp include/Players/Outside_Communicator.h src/Players/CECP_Mediator.cpp include/Players/CECP_Mediator.h src/Players/Minimax_AI.cpp include/Players/Minimax_AI.h src/Players/Player.cpp include/Players/Player.h src/Players/Random_AI.cpp include/Players/Random_AI.h src/Players/Genetic_AI.cpp include/Players/Genetic_AI.h src/Breeding/Gene_Pool.cpp include/Breeding/Gene_Pool.h src/Exceptions/Game_Ended.cpp include/Exceptions/Game_Ended.h src/Exceptions/Bad_Still_Alive_Line.cpp include/Exceptions/Bad_Still_Alive_Line.h src/Utility/Random.cpp include/Utility/Random.h src/Utility/Scoped_Stopwatch.cpp include/Utility/Scoped_Stopwatch.h src/Utility/Configuration.cpp include/Utility/Configuration.h src/Utility/Math.cpp include/Utility/Math.h src/Utility/String.cpp include/Utility/String.h src/Game/Game.cpp include/Game/Game.h src/Game/Piece.cpp include/Game/Piece.h src/Game/Square.cpp include/Game/Square.h src/Game/Board.cpp include/Game/Board.h src/Game/Game_Result.cpp include/Game/Game_Result.h src/Game/Color.cpp include/Game/Color.h src/Game/Clock.cpp include/Game/Clock.h src/Genes/Pawn_Advancement_Gene.cpp include/Genes/Pawn_Advancement_Gene.h src/Genes/King_Confinement_Gene.cpp include/Genes/King_Confinement_Gene.h src/Genes/Genome.cpp include/Genes/Genome.h src/Genes/Piece_Strength_Gene.cpp include/Genes/Piece_Strength_Gene.h src/Genes/Null_Gene.cpp include/Genes/Null_Gene.h src/Genes/Freedom_To_Move_Gene.cpp include/Genes/Freedom_To_Move_Gene.h src/Genes/Stacked_Pawns_Gene.cpp include/Genes/Stacked_Pawns_Gene.h src/Genes/Sphere_of_Influence_Gene.cpp include/Genes/Sphere_of_Influence_Gene.h src/Genes/Total_Force_Gene.cpp include/Genes/Total_Force_Gene.h src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Genes/Opponent_Pieces_Targeted_Gene.h src/Genes/King_Protection_Gene.cpp include/Genes/King_Protection_Gene.h src/Genes/Checkmate_Material_Gene.cpp include/Genes/Checkmate_Material_Gene.h src/Genes/Mutation_Rate_Gene.cpp include/Genes/Mutation_Rate_Gene.h src/Genes/Pawn_Islands_Gene.cpp include/Genes/Pawn_Islands_Gene.h src/Genes/Gene.cpp include/Genes/Gene.h src/Genes/Castling_Possible_Gene.cpp include/Genes/Castling_Possible_Gene.h src/Genes/Look_Ahead_Gene.cpp include/Genes/Look_Ahead_Gene.h src/Genes/Passed_Pawn_Gene.cpp include/Genes/Passed_Pawn_Gene.h src/Moves/Pawn_Capture.cpp include/Moves/Pawn_Capture.h src/Moves/Pawn_Promotion_by_Capture.cpp include/Moves/Pawn_Promotion_by_Capture.h src/Moves/Pawn_Promotion.cpp include/Moves/Pawn_Promotion.h src/Moves/Move.cpp include/Moves/Move.h src/Moves/Castle.cpp include/Moves/Castle.h src/Moves/Pawn_Move.cpp include/Moves/Pawn_Move.h src/Moves/En_Passant.cpp include/Moves/En_Passant.h src/Moves/Pawn_Double_Move.cpp include/Moves/Pawn_Double_Move.h

RELEASE_BIN_DIR = bin/gcc/release
OUT_RELEASE = $(RELEASE_BIN_DIR)/$(BIN)
LINK_DIR_RELEASE = bin/release
RELEASE_OBJ_DIR = obj/gcc/release
OBJ_RELEASE = $(RELEASE_OBJ_DIR)/main.o $(RELEASE_OBJ_DIR)/src/Breeding/Gene_Pool.o $(RELEASE_OBJ_DIR)/src/Exceptions/Bad_Still_Alive_Line.o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ended.o $(RELEASE_OBJ_DIR)/src/Game/Board.o $(RELEASE_OBJ_DIR)/src/Game/Clock.o $(RELEASE_OBJ_DIR)/src/Game/Color.o $(RELEASE_OBJ_DIR)/src/Game/Game.o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o $(RELEASE_OBJ_DIR)/src/Game/Piece.o $(RELEASE_OBJ_DIR)/src/Game/Square.o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Checkmate_Material_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Mutation_Rate_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Null_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(RELEASE_OBJ_DIR)/src/Moves/Castle.o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o $(RELEASE_OBJ_DIR)/src/Moves/Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o $(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o $(RELEASE_OBJ_DIR)/src/Players/Minimax_AI.o $(RELEASE_OBJ_DIR)/src/Players/Outside_Communicator.o $(RELEASE_OBJ_DIR)/src/Players/Player.o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o $(RELEASE_OBJ_DIR)/src/Testing.o $(RELEASE_OBJ_DIR)/src/Utility/Configuration.o $(RELEASE_OBJ_DIR)/src/Utility/Math.o $(RELEASE_OBJ_DIR)/src/Utility/Random.o $(RELEASE_OBJ_DIR)/src/Utility/Scoped_Stopwatch.o $(RELEASE_OBJ_DIR)/src/Utility/String.o
CFLAGS_RELEASE = -O3 -DNDEBUG
LDFLAGS_RELEASE = -flto


DEBUG_BIN_DIR = bin/gcc/debug
OUT_DEBUG = $(DEBUG_BIN_DIR)/$(BIN)
LINK_DIR_DEBUG = bin/debug
DEBUG_OBJ_DIR = obj/gcc/debug
OBJ_DEBUG = $(DEBUG_OBJ_DIR)/main.o $(DEBUG_OBJ_DIR)/src/Breeding/Gene_Pool.o $(DEBUG_OBJ_DIR)/src/Exceptions/Bad_Still_Alive_Line.o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ended.o $(DEBUG_OBJ_DIR)/src/Game/Board.o $(DEBUG_OBJ_DIR)/src/Game/Clock.o $(DEBUG_OBJ_DIR)/src/Game/Color.o $(DEBUG_OBJ_DIR)/src/Game/Game.o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o $(DEBUG_OBJ_DIR)/src/Game/Piece.o $(DEBUG_OBJ_DIR)/src/Game/Square.o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Checkmate_Material_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Mutation_Rate_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Null_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(DEBUG_OBJ_DIR)/src/Moves/Castle.o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o $(DEBUG_OBJ_DIR)/src/Moves/Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o $(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o $(DEBUG_OBJ_DIR)/src/Players/Minimax_AI.o $(DEBUG_OBJ_DIR)/src/Players/Outside_Communicator.o $(DEBUG_OBJ_DIR)/src/Players/Player.o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o $(DEBUG_OBJ_DIR)/src/Testing.o $(DEBUG_OBJ_DIR)/src/Utility/Configuration.o $(DEBUG_OBJ_DIR)/src/Utility/Math.o $(DEBUG_OBJ_DIR)/src/Utility/Random.o $(DEBUG_OBJ_DIR)/src/Utility/Scoped_Stopwatch.o $(DEBUG_OBJ_DIR)/src/Utility/String.o
CFLAGS_DEBUG = -g
LDFLAGS_DEBUG = 


all : release debug


LINK : 


after_debug : $(LINK_DIR_DEBUG)/$(BIN) $(DOC_INDEX)


after_release : $(LINK_DIR_RELEASE)/$(BIN) $(DOC_INDEX)


before_debug : 
	mkdir -p $(DEBUG_BIN_DIR)
	mkdir -p $(LINK_DIR_DEBUG)
	mkdir -p $(DEBUG_OBJ_DIR)
	mkdir -p $(DEBUG_OBJ_DIR)/src
	mkdir -p $(DEBUG_OBJ_DIR)/src/Players
	mkdir -p $(DEBUG_OBJ_DIR)/src/Breeding
	mkdir -p $(DEBUG_OBJ_DIR)/src/Exceptions
	mkdir -p $(DEBUG_OBJ_DIR)/src/Utility
	mkdir -p $(DEBUG_OBJ_DIR)/src/Game
	mkdir -p $(DEBUG_OBJ_DIR)/src/Genes
	mkdir -p $(DEBUG_OBJ_DIR)/src/Moves

before_release : 
	mkdir -p $(RELEASE_BIN_DIR)
	mkdir -p $(LINK_DIR_RELEASE)
	mkdir -p $(RELEASE_OBJ_DIR)
	mkdir -p $(RELEASE_OBJ_DIR)/src
	mkdir -p $(RELEASE_OBJ_DIR)/src/Players
	mkdir -p $(RELEASE_OBJ_DIR)/src/Breeding
	mkdir -p $(RELEASE_OBJ_DIR)/src/Exceptions
	mkdir -p $(RELEASE_OBJ_DIR)/src/Utility
	mkdir -p $(RELEASE_OBJ_DIR)/src/Game
	mkdir -p $(RELEASE_OBJ_DIR)/src/Genes
	mkdir -p $(RELEASE_OBJ_DIR)/src/Moves

clean : clean_release clean_debug
	rm -rf $(DOC_DIR)

clean_debug : 
	rm -rf $(DEBUG_OBJ_DIR) $(DEBUG_BIN_DIR)

clean_release : 
	rm -rf $(RELEASE_OBJ_DIR) $(RELEASE_BIN_DIR)

debug : before_debug $(OUT_DEBUG) after_debug


release : before_release $(OUT_RELEASE) after_release


test_all : test_release test_debug


test_debug : debug
	$(DEBUG_BIN_DIR)/$(BIN) -test
	$(DEBUG_BIN_DIR)/$(BIN) -perft
	$(DEBUG_BIN_DIR)/$(BIN) -speed

test_release : release
	$(RELEASE_BIN_DIR)/$(BIN) -test
	$(RELEASE_BIN_DIR)/$(BIN) -perft
	$(RELEASE_BIN_DIR)/$(BIN) -speed

$(DEBUG_OBJ_DIR)/main.o : main.cpp include/Breeding/Gene_Pool.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/Move.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Testing.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c main.cpp -o $(DEBUG_OBJ_DIR)/main.o

$(DEBUG_OBJ_DIR)/src/Breeding/Gene_Pool.o : src/Breeding/Gene_Pool.cpp include/Breeding/Gene_Pool.h include/Exceptions/Bad_Still_Alive_Line.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility/Configuration.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Breeding/Gene_Pool.cpp -o $(DEBUG_OBJ_DIR)/src/Breeding/Gene_Pool.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Bad_Still_Alive_Line.o : src/Exceptions/Bad_Still_Alive_Line.cpp include/Exceptions/Bad_Still_Alive_Line.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Exceptions/Bad_Still_Alive_Line.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Bad_Still_Alive_Line.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ended.o : src/Exceptions/Game_Ended.cpp include/Exceptions/Game_Ended.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Exceptions/Game_Ended.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ended.o

$(DEBUG_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Players/Player.h include/Players/Thinking.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Board.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Board.o

$(DEBUG_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Clock.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Clock.o

$(DEBUG_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Color.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Color.o

$(DEBUG_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Exceptions/Game_Ended.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Outside_Communicator.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Game.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game.o

$(DEBUG_OBJ_DIR)/src/Game/Game_Result.o : src/Game/Game_Result.cpp include/Game/Color.h include/Game/Game_Result.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Game_Result.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o

$(DEBUG_OBJ_DIR)/src/Game/Piece.o : src/Game/Piece.cpp include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Piece.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Piece.o

$(DEBUG_OBJ_DIR)/src/Game/Square.o : src/Game/Square.cpp include/Game/Color.h include/Game/Square.h include/Utility/Math.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Square.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Square.o

$(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Moves/Move.h include/Players/Thinking.h include/Utility/Math.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Castling_Possible_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Checkmate_Material_Gene.o : src/Genes/Checkmate_Material_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Checkmate_Material_Gene.h include/Genes/Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Checkmate_Material_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Checkmate_Material_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Exceptions/Genetic_AI_Creation_Error.h include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Players/Thinking.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Exceptions/Genetic_AI_Creation_Error.h include/Game/Color.h include/Game/Piece.h include/Genes/Castling_Possible_Gene.h include/Genes/Checkmate_Material_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/Mutation_Rate_Gene.h include/Genes/Null_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Genome.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Confinement_Gene.h include/Players/Thinking.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/King_Confinement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Protection_Gene.h include/Moves/Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/King_Protection_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Look_Ahead_Gene.h include/Players/Thinking.h include/Utility/Math.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Look_Ahead_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Mutation_Rate_Gene.o : src/Genes/Mutation_Rate_Gene.cpp include/Game/Color.h include/Genes/Gene.h include/Genes/Mutation_Rate_Gene.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Mutation_Rate_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Mutation_Rate_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Null_Gene.o : src/Genes/Null_Gene.cpp include/Game/Color.h include/Genes/Gene.h include/Genes/Null_Gene.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Null_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Null_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o : src/Genes/Passed_Pawn_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Passed_Pawn_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Passed_Pawn_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Advancement_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o : src/Genes/Pawn_Islands_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Islands_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Pawn_Islands_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Game/Color.h include/Game/Piece.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Piece_Strength_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Players/Thinking.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o : src/Genes/Stacked_Pawns_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Stacked_Pawns_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Stacked_Pawns_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Total_Force_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Total_Force_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(DEBUG_OBJ_DIR)/src/Moves/Castle.o : src/Moves/Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Castle.o

$(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/En_Passant.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o

$(DEBUG_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Double_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Promotion.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Exceptions/Game_Ended.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/CECP_Mediator.h include/Players/Outside_Communicator.h include/Players/Player.h include/Players/Thinking.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/CECP_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o

$(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o : src/Players/Game_Tree_Node_Result.cpp include/Game/Color.h include/Players/Game_Tree_Node_Result.h include/Utility/Math.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Game_Tree_Node_Result.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o

$(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Exceptions/Genetic_AI_Creation_Error.h include/Game/Color.h include/Genes/Gene.h include/Genes/Genome.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Genetic_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Minimax_AI.o : src/Players/Minimax_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Game_Tree_Node_Result.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Minimax_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Minimax_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Outside_Communicator.o : src/Players/Outside_Communicator.cpp include/Exceptions/Game_Ended.h include/Players/CECP_Mediator.h include/Players/Outside_Communicator.h include/Players/UCI_Mediator.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Outside_Communicator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Outside_Communicator.o

$(DEBUG_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Player.o

$(DEBUG_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Random_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o

$(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o : src/Players/UCI_Mediator.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Outside_Communicator.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/UCI_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o

$(DEBUG_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Checkmate_Material_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Null_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Moves/Move.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Testing.h include/Utility/Math.h include/Utility/Random.h include/Utility/Scoped_Stopwatch.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Testing.cpp -o $(DEBUG_OBJ_DIR)/src/Testing.o

$(DEBUG_OBJ_DIR)/src/Utility/Configuration.o : src/Utility/Configuration.cpp include/Utility/Configuration.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Utility/Configuration.cpp -o $(DEBUG_OBJ_DIR)/src/Utility/Configuration.o

$(DEBUG_OBJ_DIR)/src/Utility/Math.o : src/Utility/Math.cpp include/Utility/Math.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Utility/Math.cpp -o $(DEBUG_OBJ_DIR)/src/Utility/Math.o

$(DEBUG_OBJ_DIR)/src/Utility/Random.o : src/Utility/Random.cpp include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Utility/Random.cpp -o $(DEBUG_OBJ_DIR)/src/Utility/Random.o

$(DEBUG_OBJ_DIR)/src/Utility/Scoped_Stopwatch.o : src/Utility/Scoped_Stopwatch.cpp include/Utility/Random.h include/Utility/Scoped_Stopwatch.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Utility/Scoped_Stopwatch.cpp -o $(DEBUG_OBJ_DIR)/src/Utility/Scoped_Stopwatch.o

$(DEBUG_OBJ_DIR)/src/Utility/String.o : src/Utility/String.cpp include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Utility/String.cpp -o $(DEBUG_OBJ_DIR)/src/Utility/String.o

$(DOC_INDEX) : $(ALL_SOURCES)
	doxygen

$(LINK_DIR_DEBUG)/$(BIN) : $(OUT_DEBUG) LINK
	ln -sf -t $(LINK_DIR_DEBUG) `realpath $(OUT_DEBUG)`

$(LINK_DIR_RELEASE)/$(BIN) : $(OUT_RELEASE) LINK
	ln -sf -t $(LINK_DIR_RELEASE) `realpath $(OUT_RELEASE)`

$(OUT_DEBUG) : $(OBJ_DEBUG)
	$(LD) -o $(OUT_DEBUG) $(OBJ_DEBUG) $(LDFLAGS) $(LDFLAGS_DEBUG) $(CFLAGS) $(CFLAGS_DEBUG)

$(OUT_RELEASE) : $(OBJ_RELEASE)
	$(LD) -o $(OUT_RELEASE) $(OBJ_RELEASE) $(LDFLAGS) $(LDFLAGS_RELEASE) $(CFLAGS) $(CFLAGS_RELEASE)

$(RELEASE_OBJ_DIR)/main.o : main.cpp include/Breeding/Gene_Pool.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/Move.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Testing.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c main.cpp -o $(RELEASE_OBJ_DIR)/main.o

$(RELEASE_OBJ_DIR)/src/Breeding/Gene_Pool.o : src/Breeding/Gene_Pool.cpp include/Breeding/Gene_Pool.h include/Exceptions/Bad_Still_Alive_Line.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility/Configuration.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Breeding/Gene_Pool.cpp -o $(RELEASE_OBJ_DIR)/src/Breeding/Gene_Pool.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Bad_Still_Alive_Line.o : src/Exceptions/Bad_Still_Alive_Line.cpp include/Exceptions/Bad_Still_Alive_Line.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Exceptions/Bad_Still_Alive_Line.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Bad_Still_Alive_Line.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ended.o : src/Exceptions/Game_Ended.cpp include/Exceptions/Game_Ended.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Exceptions/Game_Ended.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ended.o

$(RELEASE_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Players/Player.h include/Players/Thinking.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Board.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Board.o

$(RELEASE_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Clock.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Clock.o

$(RELEASE_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Color.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Color.o

$(RELEASE_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Exceptions/Game_Ended.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Outside_Communicator.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Game.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game.o

$(RELEASE_OBJ_DIR)/src/Game/Game_Result.o : src/Game/Game_Result.cpp include/Game/Color.h include/Game/Game_Result.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Game_Result.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o

$(RELEASE_OBJ_DIR)/src/Game/Piece.o : src/Game/Piece.cpp include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Piece.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Piece.o

$(RELEASE_OBJ_DIR)/src/Game/Square.o : src/Game/Square.cpp include/Game/Color.h include/Game/Square.h include/Utility/Math.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Square.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Square.o

$(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Moves/Move.h include/Players/Thinking.h include/Utility/Math.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Castling_Possible_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Checkmate_Material_Gene.o : src/Genes/Checkmate_Material_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Checkmate_Material_Gene.h include/Genes/Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Checkmate_Material_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Checkmate_Material_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Exceptions/Genetic_AI_Creation_Error.h include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Players/Thinking.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Exceptions/Genetic_AI_Creation_Error.h include/Game/Color.h include/Game/Piece.h include/Genes/Castling_Possible_Gene.h include/Genes/Checkmate_Material_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/Mutation_Rate_Gene.h include/Genes/Null_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Genome.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Confinement_Gene.h include/Players/Thinking.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/King_Confinement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Protection_Gene.h include/Moves/Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/King_Protection_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Look_Ahead_Gene.h include/Players/Thinking.h include/Utility/Math.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Look_Ahead_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Mutation_Rate_Gene.o : src/Genes/Mutation_Rate_Gene.cpp include/Game/Color.h include/Genes/Gene.h include/Genes/Mutation_Rate_Gene.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Mutation_Rate_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Mutation_Rate_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Null_Gene.o : src/Genes/Null_Gene.cpp include/Game/Color.h include/Genes/Gene.h include/Genes/Null_Gene.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Null_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Null_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o : src/Genes/Passed_Pawn_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Passed_Pawn_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Passed_Pawn_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Advancement_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o : src/Genes/Pawn_Islands_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Islands_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Pawn_Islands_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Game/Color.h include/Game/Piece.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Piece_Strength_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Players/Thinking.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o : src/Genes/Stacked_Pawns_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Stacked_Pawns_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Stacked_Pawns_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Total_Force_Gene.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Total_Force_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(RELEASE_OBJ_DIR)/src/Moves/Castle.o : src/Moves/Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Castle.o

$(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/En_Passant.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o

$(RELEASE_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Double_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Promotion.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Exceptions/Game_Ended.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/CECP_Mediator.h include/Players/Outside_Communicator.h include/Players/Player.h include/Players/Thinking.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/CECP_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o

$(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o : src/Players/Game_Tree_Node_Result.cpp include/Game/Color.h include/Players/Game_Tree_Node_Result.h include/Utility/Math.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Game_Tree_Node_Result.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o

$(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Exceptions/Genetic_AI_Creation_Error.h include/Game/Color.h include/Genes/Gene.h include/Genes/Genome.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Genetic_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Minimax_AI.o : src/Players/Minimax_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Game_Tree_Node_Result.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility/Random.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Minimax_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Minimax_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Outside_Communicator.o : src/Players/Outside_Communicator.cpp include/Exceptions/Game_Ended.h include/Players/CECP_Mediator.h include/Players/Outside_Communicator.h include/Players/UCI_Mediator.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Outside_Communicator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Outside_Communicator.o

$(RELEASE_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Player.o

$(RELEASE_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Random_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o

$(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o : src/Players/UCI_Mediator.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Moves/Move.h include/Players/Outside_Communicator.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/UCI_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o

$(RELEASE_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Piece.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Checkmate_Material_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Null_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Moves/Move.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Testing.h include/Utility/Math.h include/Utility/Random.h include/Utility/Scoped_Stopwatch.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Testing.cpp -o $(RELEASE_OBJ_DIR)/src/Testing.o

$(RELEASE_OBJ_DIR)/src/Utility/Configuration.o : src/Utility/Configuration.cpp include/Utility/Configuration.h include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Utility/Configuration.cpp -o $(RELEASE_OBJ_DIR)/src/Utility/Configuration.o

$(RELEASE_OBJ_DIR)/src/Utility/Math.o : src/Utility/Math.cpp include/Utility/Math.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Utility/Math.cpp -o $(RELEASE_OBJ_DIR)/src/Utility/Math.o

$(RELEASE_OBJ_DIR)/src/Utility/Random.o : src/Utility/Random.cpp include/Utility/Random.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Utility/Random.cpp -o $(RELEASE_OBJ_DIR)/src/Utility/Random.o

$(RELEASE_OBJ_DIR)/src/Utility/Scoped_Stopwatch.o : src/Utility/Scoped_Stopwatch.cpp include/Utility/Random.h include/Utility/Scoped_Stopwatch.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Utility/Scoped_Stopwatch.cpp -o $(RELEASE_OBJ_DIR)/src/Utility/Scoped_Stopwatch.o

$(RELEASE_OBJ_DIR)/src/Utility/String.o : src/Utility/String.cpp include/Utility/String.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Utility/String.cpp -o $(RELEASE_OBJ_DIR)/src/Utility/String.o

.PHONY : all LINK after_debug after_release before_debug before_release clean clean_debug clean_release debug release test_all test_debug test_release


