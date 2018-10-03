CXX = g++
LD = g++

CFLAGS = -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wzero-as-null-pointer-constant -Wmain -pedantic -Wextra -Wall -Iinclude
LDFLAGS = -pthread -fexceptions

RELEASE_BIN_DIR = bin/release
OUT_RELEASE = $(RELEASE_BIN_DIR)/genetic_chess
RELEASE_OBJ_DIR = obj/release
OBJ_RELEASE = $(RELEASE_OBJ_DIR)/main.o $(RELEASE_OBJ_DIR)/src/Breeding/Gene_Pool.o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ended.o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move.o $(RELEASE_OBJ_DIR)/src/Exceptions/Promotion_Piece_Needed.o $(RELEASE_OBJ_DIR)/src/Game/Board.o $(RELEASE_OBJ_DIR)/src/Game/Clock.o $(RELEASE_OBJ_DIR)/src/Game/Color.o $(RELEASE_OBJ_DIR)/src/Game/Game.o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o $(RELEASE_OBJ_DIR)/src/Game/Square.o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Priority_Threshold_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(RELEASE_OBJ_DIR)/src/Moves/Castle.o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o $(RELEASE_OBJ_DIR)/src/Moves/Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Threat_Generator.o $(RELEASE_OBJ_DIR)/src/Moves/Threat_Iterator.o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o $(RELEASE_OBJ_DIR)/src/Pieces/King.o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o $(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o $(RELEASE_OBJ_DIR)/src/Players/Minimax_AI.o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o $(RELEASE_OBJ_DIR)/src/Players/Player.o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o $(RELEASE_OBJ_DIR)/src/Testing.o $(RELEASE_OBJ_DIR)/src/Utility.o
CFLAGS_RELEASE = -s -O3 -DNDEBUG
LDFLAGS_RELEASE = -flto -fuse-linker-plugin


DEBUG_BIN_DIR = bin/debug
OUT_DEBUG = $(DEBUG_BIN_DIR)/genetic_chess
DEBUG_OBJ_DIR = obj/debug
OBJ_DEBUG = $(DEBUG_OBJ_DIR)/main.o $(DEBUG_OBJ_DIR)/src/Breeding/Gene_Pool.o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ended.o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move.o $(DEBUG_OBJ_DIR)/src/Exceptions/Promotion_Piece_Needed.o $(DEBUG_OBJ_DIR)/src/Game/Board.o $(DEBUG_OBJ_DIR)/src/Game/Clock.o $(DEBUG_OBJ_DIR)/src/Game/Color.o $(DEBUG_OBJ_DIR)/src/Game/Game.o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o $(DEBUG_OBJ_DIR)/src/Game/Square.o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Priority_Threshold_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(DEBUG_OBJ_DIR)/src/Moves/Castle.o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o $(DEBUG_OBJ_DIR)/src/Moves/Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Threat_Generator.o $(DEBUG_OBJ_DIR)/src/Moves/Threat_Iterator.o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o $(DEBUG_OBJ_DIR)/src/Pieces/King.o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o $(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o $(DEBUG_OBJ_DIR)/src/Players/Minimax_AI.o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o $(DEBUG_OBJ_DIR)/src/Players/Player.o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o $(DEBUG_OBJ_DIR)/src/Testing.o $(DEBUG_OBJ_DIR)/src/Utility.o
CFLAGS_DEBUG = -g -DDEBUG
LDFLAGS_DEBUG = 


all : release debug


before_debug : 
	mkdir -p $(DEBUG_BIN_DIR)
	mkdir -p $(DEBUG_OBJ_DIR)
	mkdir -p $(DEBUG_OBJ_DIR)/src
	mkdir -p $(DEBUG_OBJ_DIR)/src/Breeding
	mkdir -p $(DEBUG_OBJ_DIR)/src/Genes
	mkdir -p $(DEBUG_OBJ_DIR)/src/Exceptions
	mkdir -p $(DEBUG_OBJ_DIR)/src/Game
	mkdir -p $(DEBUG_OBJ_DIR)/src/Moves
	mkdir -p $(DEBUG_OBJ_DIR)/src/Players
	mkdir -p $(DEBUG_OBJ_DIR)/src/Pieces

before_release : 
	mkdir -p $(RELEASE_BIN_DIR)
	mkdir -p $(RELEASE_OBJ_DIR)
	mkdir -p $(RELEASE_OBJ_DIR)/src
	mkdir -p $(RELEASE_OBJ_DIR)/src/Breeding
	mkdir -p $(RELEASE_OBJ_DIR)/src/Genes
	mkdir -p $(RELEASE_OBJ_DIR)/src/Exceptions
	mkdir -p $(RELEASE_OBJ_DIR)/src/Game
	mkdir -p $(RELEASE_OBJ_DIR)/src/Moves
	mkdir -p $(RELEASE_OBJ_DIR)/src/Players
	mkdir -p $(RELEASE_OBJ_DIR)/src/Pieces

clean : clean_release clean_debug


clean_debug : 
	rm -rf $(DEBUG_OBJ_DIR) $(DEBUG_BIN_DIR)

clean_release : 
	rm -rf $(RELEASE_OBJ_DIR) $(RELEASE_BIN_DIR)

debug : before_debug $(OUT_DEBUG)


release : before_release $(OUT_RELEASE)


$(DEBUG_OBJ_DIR)/main.o : main.cpp include/Breeding/Gene_Pool.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Minimax_AI.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c main.cpp -o $(DEBUG_OBJ_DIR)/main.o

$(DEBUG_OBJ_DIR)/src/Breeding/Gene_Pool.o : src/Breeding/Gene_Pool.cpp include/Breeding/Gene_Pool.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Genes/Gene.h include/Genes/Genome.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Breeding/Gene_Pool.cpp -o $(DEBUG_OBJ_DIR)/src/Breeding/Gene_Pool.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ended.o : src/Exceptions/Game_Ended.cpp include/Exceptions/Game_Ended.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Exceptions/Game_Ended.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ended.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move.o : src/Exceptions/Illegal_Move.cpp include/Exceptions/Illegal_Move.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Exceptions/Illegal_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Promotion_Piece_Needed.o : src/Exceptions/Promotion_Piece_Needed.cpp include/Exceptions/Promotion_Piece_Needed.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Exceptions/Promotion_Piece_Needed.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Promotion_Piece_Needed.o

$(DEBUG_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Exceptions/Illegal_Move.h include/Exceptions/Promotion_Piece_Needed.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Threat_Generator.h include/Moves/Threat_Iterator.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Board.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Board.o

$(DEBUG_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Clock.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Clock.o

$(DEBUG_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Color.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Color.o

$(DEBUG_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Exceptions/Game_Ended.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Game.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game.o

$(DEBUG_OBJ_DIR)/src/Game/Game_Result.o : src/Game/Game_Result.cpp include/Game/Color.h include/Game/Game_Result.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Game_Result.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o

$(DEBUG_OBJ_DIR)/src/Game/Square.o : src/Game/Square.cpp include/Game/Square.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Square.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Square.o

$(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Castling_Possible_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Priority_Threshold_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Genome.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Confinement_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/King_Confinement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Protection_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/King_Protection_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Look_Ahead_Gene.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Look_Ahead_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o : src/Genes/Passed_Pawn_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Passed_Pawn_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Passed_Pawn_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Advancement_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o : src/Genes/Pawn_Islands_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Islands_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Pawn_Islands_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Piece_Strength_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Priority_Threshold_Gene.o : src/Genes/Priority_Threshold_Gene.cpp include/Genes/Gene.h include/Genes/Priority_Threshold_Gene.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Priority_Threshold_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Priority_Threshold_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o : src/Genes/Stacked_Pawns_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Stacked_Pawns_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Stacked_Pawns_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Total_Force_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Total_Force_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(DEBUG_OBJ_DIR)/src/Moves/Castle.o : src/Moves/Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Castle.o

$(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/En_Passant.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o

$(DEBUG_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Double_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Promotion.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Threat_Generator.o : src/Moves/Threat_Generator.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Threat_Generator.h include/Moves/Threat_Iterator.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Threat_Generator.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Threat_Generator.o

$(DEBUG_OBJ_DIR)/src/Moves/Threat_Iterator.o : src/Moves/Threat_Iterator.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Threat_Iterator.h include/Pieces/Knight.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Threat_Iterator.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Threat_Iterator.o

$(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o : src/Pieces/Bishop.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Bishop.h include/Pieces/Piece.h include/Pieces/Piece_Types.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Bishop.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o

$(DEBUG_OBJ_DIR)/src/Pieces/King.o : src/Pieces/King.cpp include/Game/Color.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Pieces/King.h include/Pieces/Piece.h include/Pieces/Piece_Types.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/King.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/King.o

$(DEBUG_OBJ_DIR)/src/Pieces/Knight.o : src/Pieces/Knight.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Knight.h include/Pieces/Piece.h include/Pieces/Piece_Types.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Knight.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o

$(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o : src/Pieces/Pawn.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Pieces/Bishop.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Pawn.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o

$(DEBUG_OBJ_DIR)/src/Pieces/Piece.o : src/Pieces/Piece.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Piece.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o

$(DEBUG_OBJ_DIR)/src/Pieces/Queen.o : src/Pieces/Queen.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Queen.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Queen.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o

$(DEBUG_OBJ_DIR)/src/Pieces/Rook.o : src/Pieces/Rook.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Rook.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Rook.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o

$(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Exceptions/Game_Ended.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/CECP_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o

$(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o : src/Players/Game_Tree_Node_Result.cpp include/Game/Color.h include/Players/Game_Tree_Node_Result.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Game_Tree_Node_Result.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o

$(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Genetic_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Human_Player.o : src/Players/Human_Player.cpp include/Exceptions/Illegal_Move.h include/Exceptions/Promotion_Piece_Needed.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Human_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Human_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Minimax_AI.o : src/Players/Minimax_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Game_Tree_Node_Result.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Minimax_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Minimax_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o : src/Players/Outside_Player.cpp include/Exceptions/Game_Ended.h include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Outside_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Player.o

$(DEBUG_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Random_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o

$(DEBUG_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Testing.cpp -o $(DEBUG_OBJ_DIR)/src/Testing.o

$(DEBUG_OBJ_DIR)/src/Utility.o : src/Utility.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Utility.cpp -o $(DEBUG_OBJ_DIR)/src/Utility.o

$(OUT_DEBUG) : $(OBJ_DEBUG)
	$(LD) -o $(OUT_DEBUG) $(OBJ_DEBUG) $(LDFLAGS) $(LDFLAGS_DEBUG) $(CFLAGS) $(CFLAGS_DEBUG)

$(OUT_RELEASE) : $(OBJ_RELEASE)
	$(LD) -o $(OUT_RELEASE) $(OBJ_RELEASE) $(LDFLAGS) $(LDFLAGS_RELEASE) $(CFLAGS) $(CFLAGS_RELEASE)

$(RELEASE_OBJ_DIR)/main.o : main.cpp include/Breeding/Gene_Pool.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Minimax_AI.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c main.cpp -o $(RELEASE_OBJ_DIR)/main.o

$(RELEASE_OBJ_DIR)/src/Breeding/Gene_Pool.o : src/Breeding/Gene_Pool.cpp include/Breeding/Gene_Pool.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Genes/Gene.h include/Genes/Genome.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Breeding/Gene_Pool.cpp -o $(RELEASE_OBJ_DIR)/src/Breeding/Gene_Pool.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ended.o : src/Exceptions/Game_Ended.cpp include/Exceptions/Game_Ended.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Exceptions/Game_Ended.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ended.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move.o : src/Exceptions/Illegal_Move.cpp include/Exceptions/Illegal_Move.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Exceptions/Illegal_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Promotion_Piece_Needed.o : src/Exceptions/Promotion_Piece_Needed.cpp include/Exceptions/Promotion_Piece_Needed.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Exceptions/Promotion_Piece_Needed.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Promotion_Piece_Needed.o

$(RELEASE_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Exceptions/Illegal_Move.h include/Exceptions/Promotion_Piece_Needed.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Threat_Generator.h include/Moves/Threat_Iterator.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Board.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Board.o

$(RELEASE_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Clock.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Clock.o

$(RELEASE_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Color.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Color.o

$(RELEASE_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Exceptions/Game_Ended.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Game.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game.o

$(RELEASE_OBJ_DIR)/src/Game/Game_Result.o : src/Game/Game_Result.cpp include/Game/Color.h include/Game/Game_Result.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Game_Result.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o

$(RELEASE_OBJ_DIR)/src/Game/Square.o : src/Game/Square.cpp include/Game/Square.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Square.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Square.o

$(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Castling_Possible_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Priority_Threshold_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Genome.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Confinement_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/King_Confinement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Protection_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/King_Protection_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Look_Ahead_Gene.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Look_Ahead_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o : src/Genes/Passed_Pawn_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Passed_Pawn_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Passed_Pawn_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Passed_Pawn_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Advancement_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o : src/Genes/Pawn_Islands_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Islands_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Pawn_Islands_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Islands_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Piece_Strength_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Priority_Threshold_Gene.o : src/Genes/Priority_Threshold_Gene.cpp include/Genes/Gene.h include/Genes/Priority_Threshold_Gene.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Priority_Threshold_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Priority_Threshold_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o : src/Genes/Stacked_Pawns_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Stacked_Pawns_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Stacked_Pawns_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Stacked_Pawns_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Total_Force_Gene.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Total_Force_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(RELEASE_OBJ_DIR)/src/Moves/Castle.o : src/Moves/Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Castle.o

$(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/En_Passant.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o

$(RELEASE_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Double_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Promotion.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Threat_Generator.o : src/Moves/Threat_Generator.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Threat_Generator.h include/Moves/Threat_Iterator.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Threat_Generator.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Threat_Generator.o

$(RELEASE_OBJ_DIR)/src/Moves/Threat_Iterator.o : src/Moves/Threat_Iterator.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Moves/Threat_Iterator.h include/Pieces/Knight.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Threat_Iterator.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Threat_Iterator.o

$(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o : src/Pieces/Bishop.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Bishop.h include/Pieces/Piece.h include/Pieces/Piece_Types.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Bishop.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o

$(RELEASE_OBJ_DIR)/src/Pieces/King.o : src/Pieces/King.cpp include/Game/Color.h include/Moves/Castle.h include/Moves/Direction.h include/Moves/Move.h include/Pieces/King.h include/Pieces/Piece.h include/Pieces/Piece_Types.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/King.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/King.o

$(RELEASE_OBJ_DIR)/src/Pieces/Knight.o : src/Pieces/Knight.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Knight.h include/Pieces/Piece.h include/Pieces/Piece_Types.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Knight.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o

$(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o : src/Pieces/Pawn.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Direction.h include/Moves/En_Passant.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Pieces/Bishop.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Pawn.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o

$(RELEASE_OBJ_DIR)/src/Pieces/Piece.o : src/Pieces/Piece.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Piece.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o

$(RELEASE_OBJ_DIR)/src/Pieces/Queen.o : src/Pieces/Queen.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Queen.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Queen.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o

$(RELEASE_OBJ_DIR)/src/Pieces/Rook.o : src/Pieces/Rook.cpp include/Game/Color.h include/Moves/Move.h include/Pieces/Piece.h include/Pieces/Piece_Types.h include/Pieces/Rook.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Rook.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o

$(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Exceptions/Game_Ended.h include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/CECP_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o

$(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o : src/Players/Game_Tree_Node_Result.cpp include/Game/Color.h include/Players/Game_Tree_Node_Result.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Game_Tree_Node_Result.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o

$(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Genetic_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Human_Player.o : src/Players/Human_Player.cpp include/Exceptions/Illegal_Move.h include/Exceptions/Promotion_Piece_Needed.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Human_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Human_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Minimax_AI.o : src/Players/Minimax_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Game_Tree_Node_Result.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Minimax_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Minimax_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o : src/Players/Outside_Player.cpp include/Exceptions/Game_Ended.h include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Outside_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Player.o

$(RELEASE_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Random_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o

$(RELEASE_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Exceptions/Illegal_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Passed_Pawn_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Pawn_Islands_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Stacked_Pawns_Gene.h include/Genes/Total_Force_Gene.h include/Moves/Move.h include/Pieces/Piece_Types.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Minimax_AI.h include/Players/Player.h include/Players/Thinking.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Testing.cpp -o $(RELEASE_OBJ_DIR)/src/Testing.o

$(RELEASE_OBJ_DIR)/src/Utility.o : src/Utility.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Utility.cpp -o $(RELEASE_OBJ_DIR)/src/Utility.o

.PHONY : all before_debug before_release clean clean_debug clean_release debug release


