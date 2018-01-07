CXX = g++
LD = g++

CFLAGS = -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wzero-as-null-pointer-constant -Wmain -pedantic -Wextra -Wall -Iinclude
LDFLAGS = -pthread -fexceptions

OUT_RELEASE = bin/release/genetic_chess
RELEASE_OBJ_DIR = obj/release
OBJ_RELEASE = $(RELEASE_OBJ_DIR)/main.o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o $(RELEASE_OBJ_DIR)/src/Game/Board.o $(RELEASE_OBJ_DIR)/src/Game/Clock.o $(RELEASE_OBJ_DIR)/src/Game/Color.o $(RELEASE_OBJ_DIR)/src/Game/Game.o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o $(RELEASE_OBJ_DIR)/src/Game/Square.o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o $(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o $(RELEASE_OBJ_DIR)/src/Moves/Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o $(RELEASE_OBJ_DIR)/src/Pieces/King.o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o $(RELEASE_OBJ_DIR)/src/Players/Alan_Turing_AI.o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o $(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o $(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o $(RELEASE_OBJ_DIR)/src/Players/Player.o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o $(RELEASE_OBJ_DIR)/src/Stalemate_Search.o $(RELEASE_OBJ_DIR)/src/Testing.o $(RELEASE_OBJ_DIR)/src/Utility.o
CFLAGS_RELEASE = -s -Ofast -DNDEBUG
LDFLAGS_RELEASE = -flto -fuse-linker-plugin


OUT_DEBUG = bin/debug/genetic_chess
DEBUG_OBJ_DIR = obj/debug
OBJ_DEBUG = $(DEBUG_OBJ_DIR)/main.o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o $(DEBUG_OBJ_DIR)/src/Game/Board.o $(DEBUG_OBJ_DIR)/src/Game/Clock.o $(DEBUG_OBJ_DIR)/src/Game/Color.o $(DEBUG_OBJ_DIR)/src/Game/Game.o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o $(DEBUG_OBJ_DIR)/src/Game/Square.o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o $(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o $(DEBUG_OBJ_DIR)/src/Moves/Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o $(DEBUG_OBJ_DIR)/src/Pieces/King.o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o $(DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o $(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o $(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o $(DEBUG_OBJ_DIR)/src/Players/Player.o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o $(DEBUG_OBJ_DIR)/src/Stalemate_Search.o $(DEBUG_OBJ_DIR)/src/Testing.o $(DEBUG_OBJ_DIR)/src/Utility.o
CFLAGS_DEBUG = -g -Og -DDEBUG
LDFLAGS_DEBUG = 


OUT_OPTIMIZED_DEBUG = bin/optimized_debug/genetic_chess
OPTIMIZED_DEBUG_OBJ_DIR = obj/optimized_debug
OBJ_OPTIMIZED_DEBUG = $(OPTIMIZED_DEBUG_OBJ_DIR)/main.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Board.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Clock.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Color.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Game.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Game_Result.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Square.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Genome.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/En_Passant.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Move.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Bishop.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/King.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Knight.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Pawn.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Piece.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Queen.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Rook.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Human_Player.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Outside_Player.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Player.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Random_AI.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Stalemate_Search.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Testing.o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Utility.o
CFLAGS_OPTIMIZED_DEBUG = -Ofast -DDEBUG -DNDEBUG
LDFLAGS_OPTIMIZED_DEBUG = -flto -fuse-linker-plugin


all : release debug optimized_debug


after_debug : 


after_optimized_debug : 


after_release : 


before_debug : 
	test -d bin/debug || mkdir -p bin/debug
	test -d $(DEBUG_OBJ_DIR) || mkdir -p $(DEBUG_OBJ_DIR)
	test -d $(DEBUG_OBJ_DIR)/src || mkdir -p $(DEBUG_OBJ_DIR)/src
	test -d $(DEBUG_OBJ_DIR)/src/Exceptions || mkdir -p $(DEBUG_OBJ_DIR)/src/Exceptions
	test -d $(DEBUG_OBJ_DIR)/src/Game || mkdir -p $(DEBUG_OBJ_DIR)/src/Game
	test -d $(DEBUG_OBJ_DIR)/src/Players || mkdir -p $(DEBUG_OBJ_DIR)/src/Players
	test -d $(DEBUG_OBJ_DIR)/src/Pieces || mkdir -p $(DEBUG_OBJ_DIR)/src/Pieces
	test -d $(DEBUG_OBJ_DIR)/src/Moves || mkdir -p $(DEBUG_OBJ_DIR)/src/Moves
	test -d $(DEBUG_OBJ_DIR)/src/Genes || mkdir -p $(DEBUG_OBJ_DIR)/src/Genes

before_optimized_debug : 
	test -d bin/optimized_debug || mkdir -p bin/optimized_debug
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR) || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR)/src || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)/src
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves
	test -d $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes || mkdir -p $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes

before_release : 
	test -d bin/release || mkdir -p bin/release
	test -d $(RELEASE_OBJ_DIR) || mkdir -p $(RELEASE_OBJ_DIR)
	test -d $(RELEASE_OBJ_DIR)/src || mkdir -p $(RELEASE_OBJ_DIR)/src
	test -d $(RELEASE_OBJ_DIR)/src/Exceptions || mkdir -p $(RELEASE_OBJ_DIR)/src/Exceptions
	test -d $(RELEASE_OBJ_DIR)/src/Game || mkdir -p $(RELEASE_OBJ_DIR)/src/Game
	test -d $(RELEASE_OBJ_DIR)/src/Players || mkdir -p $(RELEASE_OBJ_DIR)/src/Players
	test -d $(RELEASE_OBJ_DIR)/src/Pieces || mkdir -p $(RELEASE_OBJ_DIR)/src/Pieces
	test -d $(RELEASE_OBJ_DIR)/src/Moves || mkdir -p $(RELEASE_OBJ_DIR)/src/Moves
	test -d $(RELEASE_OBJ_DIR)/src/Genes || mkdir -p $(RELEASE_OBJ_DIR)/src/Genes

clean : clean_release clean_debug clean_optimized_debug


clean_debug : 
	rm $(OBJ_DEBUG) $(OUT_DEBUG)

clean_optimized_debug : 
	rm $(OBJ_OPTIMIZED_DEBUG) $(OUT_OPTIMIZED_DEBUG)

clean_release : 
	rm $(OBJ_RELEASE) $(OUT_RELEASE)

debug : before_debug $(OUT_DEBUG) after_debug


optimized_debug : before_optimized_debug $(OUT_OPTIMIZED_DEBUG) after_optimized_debug


release : before_release $(OUT_RELEASE) after_release


$(DEBUG_OBJ_DIR)/main.o : main.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Gene_Pool.h include/Genes/Genome.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Stalemate_Search.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c main.cpp -o $(DEBUG_OBJ_DIR)/main.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o : src/Exceptions/Game_Ending_Exception.cpp include/Exceptions/Game_Ending_Exception.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Exceptions/Game_Ending_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o : src/Exceptions/Illegal_Move_Exception.cpp include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Exceptions/Illegal_Move_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o

$(DEBUG_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Board.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Board.o

$(DEBUG_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Clock.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Clock.o

$(DEBUG_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Color.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Color.o

$(DEBUG_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Exceptions/Game_Ending_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Game.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game.o

$(DEBUG_OBJ_DIR)/src/Game/Game_Result.o : src/Game/Game_Result.cpp include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Game_Result.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o

$(DEBUG_OBJ_DIR)/src/Game/Square.o : src/Game/Square.cpp include/Game/Square.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Game/Square.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Square.o

$(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Castling_Possible_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Game/Color.h include/Genes/Gene.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o : src/Genes/Gene_Pool.cpp include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Genes/Gene.h include/Genes/Gene_Pool.h include/Genes/Genome.h include/Moves/Move.h include/Players/Genetic_AI.h include/Players/Player.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Gene_Pool.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o

$(DEBUG_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Genome.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Confinement_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/King_Confinement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Protection_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/King_Protection_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Look_Ahead_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Look_Ahead_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Advancement_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Piece_Strength_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Genes/Total_Force_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/En_Passant.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o

$(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o : src/Moves/Kingside_Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Kingside_Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o

$(DEBUG_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Double_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Promotion.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o : src/Moves/Queenside_Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Moves/Queenside_Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o

$(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o : src/Pieces/Bishop.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Bishop.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o

$(DEBUG_OBJ_DIR)/src/Pieces/King.o : src/Pieces/King.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/King.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/King.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/King.o

$(DEBUG_OBJ_DIR)/src/Pieces/Knight.o : src/Pieces/Knight.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Knight.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Knight.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o

$(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o : src/Pieces/Pawn.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Pawn.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o

$(DEBUG_OBJ_DIR)/src/Pieces/Piece.o : src/Pieces/Piece.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Piece.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o

$(DEBUG_OBJ_DIR)/src/Pieces/Queen.o : src/Pieces/Queen.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Pieces/Queen.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Queen.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o

$(DEBUG_OBJ_DIR)/src/Pieces/Rook.o : src/Pieces/Rook.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Pieces/Rook.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o

$(DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o : src/Players/Alan_Turing_AI.cpp include/Players/Alan_Turing_AI.h include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Alan_Turing_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o

$(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Exceptions/Game_Ending_Exception.h include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/CECP_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o

$(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o : src/Players/Claude_Shannon_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Claude_Shannon_AI.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Claude_Shannon_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o : src/Players/Game_Tree_Node_Result.cpp include/Game/Color.h include/Players/Game_Tree_Node_Result.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Game_Tree_Node_Result.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o

$(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Genetic_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Human_Player.o : src/Players/Human_Player.cpp include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Human_Player.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Human_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o : src/Players/Outside_Player.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Outside_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Player.o

$(DEBUG_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/Random_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o

$(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o : src/Players/UCI_Mediator.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Players/UCI_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o

$(DEBUG_OBJ_DIR)/src/Stalemate_Search.o : src/Stalemate_Search.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Stalemate_Search.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Stalemate_Search.cpp -o $(DEBUG_OBJ_DIR)/src/Stalemate_Search.o

$(DEBUG_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Player.h include/Players/Thinking.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Testing.cpp -o $(DEBUG_OBJ_DIR)/src/Testing.o

$(DEBUG_OBJ_DIR)/src/Utility.o : src/Utility.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS_DEBUG) -c src/Utility.cpp -o $(DEBUG_OBJ_DIR)/src/Utility.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/main.o : main.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Gene_Pool.h include/Genes/Genome.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Stalemate_Search.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c main.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/main.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o : src/Exceptions/Game_Ending_Exception.cpp include/Exceptions/Game_Ending_Exception.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Exceptions/Game_Ending_Exception.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o : src/Exceptions/Illegal_Move_Exception.cpp include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Exceptions/Illegal_Move_Exception.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Game/Board.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Board.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Game/Clock.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Clock.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Game/Color.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Color.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Exceptions/Game_Ending_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Game/Game.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Game.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Game_Result.o : src/Game/Game_Result.cpp include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Game/Game_Result.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Game_Result.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Square.o : src/Game/Square.cpp include/Game/Square.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Game/Square.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Game/Square.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Castling_Possible_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Game/Color.h include/Genes/Gene.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o : src/Genes/Gene_Pool.cpp include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Genes/Gene.h include/Genes/Gene_Pool.h include/Genes/Genome.h include/Moves/Move.h include/Players/Genetic_AI.h include/Players/Player.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Gene_Pool.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Genome.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Genome.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Confinement_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/King_Confinement_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Protection_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/King_Protection_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Look_Ahead_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Look_Ahead_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Advancement_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Piece_Strength_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Genes/Total_Force_Gene.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/En_Passant.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/En_Passant.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o : src/Moves/Kingside_Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Kingside_Castle.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Move.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Move.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Pawn_Capture.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Pawn_Double_Move.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Pawn_Move.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Pawn_Promotion.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o : src/Moves/Queenside_Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Moves/Queenside_Castle.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Bishop.o : src/Pieces/Bishop.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Pieces/Bishop.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Bishop.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/King.o : src/Pieces/King.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/King.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Pieces/King.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/King.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Knight.o : src/Pieces/Knight.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Knight.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Pieces/Knight.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Knight.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Pawn.o : src/Pieces/Pawn.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Pieces/Pawn.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Pawn.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Piece.o : src/Pieces/Piece.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Pieces/Piece.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Piece.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Queen.o : src/Pieces/Queen.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Pieces/Queen.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Pieces/Queen.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Queen.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Rook.o : src/Pieces/Rook.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Pieces/Rook.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Pieces/Rook.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o : src/Players/Alan_Turing_AI.cpp include/Players/Alan_Turing_AI.h include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Alan_Turing_AI.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Exceptions/Game_Ending_Exception.h include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/CECP_Mediator.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o : src/Players/Claude_Shannon_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Claude_Shannon_AI.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Claude_Shannon_AI.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o : src/Players/Game_Tree_Node_Result.cpp include/Game/Color.h include/Players/Game_Tree_Node_Result.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Game_Tree_Node_Result.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Genetic_AI.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Human_Player.o : src/Players/Human_Player.cpp include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Human_Player.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Human_Player.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Human_Player.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Outside_Player.o : src/Players/Outside_Player.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Outside_Player.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Outside_Player.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Player.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Player.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/Random_AI.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/Random_AI.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o : src/Players/UCI_Mediator.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Players/UCI_Mediator.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Stalemate_Search.o : src/Stalemate_Search.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Stalemate_Search.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Stalemate_Search.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Stalemate_Search.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Player.h include/Players/Thinking.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Testing.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Testing.o

$(OPTIMIZED_DEBUG_OBJ_DIR)/src/Utility.o : src/Utility.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_OPTIMIZED_DEBUG) $(LDFLAGS_OPTIMIZED_DEBUG) -c src/Utility.cpp -o $(OPTIMIZED_DEBUG_OBJ_DIR)/src/Utility.o

$(OUT_DEBUG) : $(OBJ_DEBUG)
	$(LD) -o $(OUT_DEBUG) $(OBJ_DEBUG) $(LDFLAGS) $(LDFLAGS_DEBUG) $(CFLAGS) $(CFLAGS_DEBUG)

$(OUT_OPTIMIZED_DEBUG) : $(OBJ_OPTIMIZED_DEBUG)
	$(LD) -o $(OUT_OPTIMIZED_DEBUG) $(OBJ_OPTIMIZED_DEBUG) $(LDFLAGS) $(LDFLAGS_OPTIMIZED_DEBUG) $(CFLAGS) $(CFLAGS_OPTIMIZED_DEBUG)

$(OUT_RELEASE) : $(OBJ_RELEASE)
	$(LD) -o $(OUT_RELEASE) $(OBJ_RELEASE) $(LDFLAGS) $(LDFLAGS_RELEASE) $(CFLAGS) $(CFLAGS_RELEASE)

$(RELEASE_OBJ_DIR)/main.o : main.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Gene_Pool.h include/Genes/Genome.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Stalemate_Search.h include/Testing.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c main.cpp -o $(RELEASE_OBJ_DIR)/main.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o : src/Exceptions/Game_Ending_Exception.cpp include/Exceptions/Game_Ending_Exception.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Exceptions/Game_Ending_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o : src/Exceptions/Illegal_Move_Exception.cpp include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Exceptions/Illegal_Move_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o

$(RELEASE_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Board.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Board.o

$(RELEASE_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Clock.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Clock.o

$(RELEASE_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Color.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Color.o

$(RELEASE_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Exceptions/Game_Ending_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Game.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game.o

$(RELEASE_OBJ_DIR)/src/Game/Game_Result.o : src/Game/Game_Result.cpp include/Game/Color.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Game_Result.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o

$(RELEASE_OBJ_DIR)/src/Game/Square.o : src/Game/Square.cpp include/Game/Square.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Game/Square.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Square.o

$(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Castling_Possible_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Game/Color.h include/Genes/Gene.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o : src/Genes/Gene_Pool.cpp include/Game/Color.h include/Game/Game.h include/Game/Game_Result.h include/Genes/Gene.h include/Genes/Gene_Pool.h include/Genes/Genome.h include/Moves/Move.h include/Players/Genetic_AI.h include/Players/Player.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Gene_Pool.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o

$(RELEASE_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Castling_Possible_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Genes/Genome.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Genome.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Confinement_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/King_Confinement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/King_Protection_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/King_Protection_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Look_Ahead_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Look_Ahead_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Pawn_Advancement_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Piece_Strength_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Piece_Strength_Gene.h include/Genes/Total_Force_Gene.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Genes/Total_Force_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/En_Passant.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o

$(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o : src/Moves/Kingside_Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Kingside_Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o

$(RELEASE_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Double_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Promotion.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o : src/Moves/Queenside_Castle.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Moves/Queenside_Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o

$(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o : src/Pieces/Bishop.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Bishop.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o

$(RELEASE_OBJ_DIR)/src/Pieces/King.o : src/Pieces/King.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/King.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/King.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/King.o

$(RELEASE_OBJ_DIR)/src/Pieces/Knight.o : src/Pieces/Knight.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Knight.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Knight.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o

$(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o : src/Pieces/Pawn.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Queenside_Castle.h include/Pieces/Bishop.h include/Pieces/Knight.h include/Pieces/Pawn.h include/Pieces/Piece.h include/Pieces/Queen.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Pawn.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o

$(RELEASE_OBJ_DIR)/src/Pieces/Piece.o : src/Pieces/Piece.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Piece.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o

$(RELEASE_OBJ_DIR)/src/Pieces/Queen.o : src/Pieces/Queen.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Pieces/Queen.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Queen.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o

$(RELEASE_OBJ_DIR)/src/Pieces/Rook.o : src/Pieces/Rook.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Pieces/Piece.h include/Pieces/Rook.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Pieces/Rook.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o

$(RELEASE_OBJ_DIR)/src/Players/Alan_Turing_AI.o : src/Players/Alan_Turing_AI.cpp include/Players/Alan_Turing_AI.h include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Alan_Turing_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Alan_Turing_AI.o

$(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Exceptions/Game_Ending_Exception.h include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/CECP_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o

$(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o : src/Players/Claude_Shannon_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Claude_Shannon_AI.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Claude_Shannon_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o : src/Players/Game_Tree_Node_Result.cpp include/Game/Color.h include/Players/Game_Tree_Node_Result.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Game_Tree_Node_Result.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Game_Tree_Node_Result.o

$(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Genes/Gene.h include/Genes/Genome.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Game_Tree_Node_Result.h include/Players/Genetic_AI.h include/Players/Player.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Genetic_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Human_Player.o : src/Players/Human_Player.cpp include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Human_Player.h include/Players/Player.h include/Players/Thinking.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Human_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o : src/Players/Outside_Player.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Outside_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Player.o

$(RELEASE_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Random_AI.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/Random_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o

$(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o : src/Players/UCI_Mediator.cpp include/Exceptions/Illegal_Move_Exception.h include/Game/Board.h include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Outside_Player.h include/Players/Player.h include/Players/Thinking.h include/Players/UCI_Mediator.h include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Players/UCI_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o

$(RELEASE_OBJ_DIR)/src/Stalemate_Search.o : src/Stalemate_Search.cpp include/Game/Board.h include/Game/Color.h include/Game/Game_Result.h include/Game/Square.h include/Moves/En_Passant.h include/Moves/Kingside_Castle.h include/Moves/Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Promotion.h include/Moves/Queenside_Castle.h include/Players/Player.h include/Players/Thinking.h include/Stalemate_Search.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Stalemate_Search.cpp -o $(RELEASE_OBJ_DIR)/src/Stalemate_Search.o

$(RELEASE_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Testing.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Testing.cpp -o $(RELEASE_OBJ_DIR)/src/Testing.o

$(RELEASE_OBJ_DIR)/src/Utility.o : src/Utility.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS_RELEASE) -c src/Utility.cpp -o $(RELEASE_OBJ_DIR)/src/Utility.o

.PHONY : all after_debug after_optimized_debug after_release before_debug before_optimized_debug before_release clean clean_debug clean_optimized_debug clean_release debug optimized_debug release


