CXX = g++
LD = g++

CFLAGS = -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wzero-as-null-pointer-constant -Wmain -pedantic -Wextra -Wall -fexceptions -Iinclude -pthread
LDFLAGS = -pthread

OUT_RELEASE = bin/release/genetic_chess
RELEASE_OBJ_DIR = obj/release
OBJ_RELEASE = $(RELEASE_OBJ_DIR)/main.o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o $(RELEASE_OBJ_DIR)/src/Game/Board.o $(RELEASE_OBJ_DIR)/src/Game/Clock.o $(RELEASE_OBJ_DIR)/src/Game/Color.o $(RELEASE_OBJ_DIR)/src/Game/Game.o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(RELEASE_OBJ_DIR)/src/Moves/Complete_Move.o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o $(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o $(RELEASE_OBJ_DIR)/src/Moves/Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o $(RELEASE_OBJ_DIR)/src/Pieces/King.o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o $(RELEASE_OBJ_DIR)/src/Players/Alan_Turing_AI.o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o $(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o $(RELEASE_OBJ_DIR)/src/Players/Player.o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o $(RELEASE_OBJ_DIR)/src/Stalemate_Search.o $(RELEASE_OBJ_DIR)/src/Testing.o $(RELEASE_OBJ_DIR)/src/Utility.o 
CFLAGS_RELEASE = -s -O2 -DRELEASE


OUT_DEBUG = bin/debug/genetic_chess
DEBUG_OBJ_DIR = obj/debug
OBJ_DEBUG = $(DEBUG_OBJ_DIR)/main.o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o $(DEBUG_OBJ_DIR)/src/Game/Board.o $(DEBUG_OBJ_DIR)/src/Game/Clock.o $(DEBUG_OBJ_DIR)/src/Game/Color.o $(DEBUG_OBJ_DIR)/src/Game/Game.o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(DEBUG_OBJ_DIR)/src/Moves/Complete_Move.o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o $(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o $(DEBUG_OBJ_DIR)/src/Moves/Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o $(DEBUG_OBJ_DIR)/src/Pieces/King.o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o $(DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o $(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o $(DEBUG_OBJ_DIR)/src/Players/Player.o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o $(DEBUG_OBJ_DIR)/src/Stalemate_Search.o $(DEBUG_OBJ_DIR)/src/Testing.o $(DEBUG_OBJ_DIR)/src/Utility.o 
CFLAGS_DEBUG = -g -DDEBUG


all : release debug


after_debug : 


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

clean : clean_release clean_debug


clean_debug : 
	rm $(OBJ_DEBUG) $(OUT_DEBUG)

clean_release : 
	rm $(OBJ_RELEASE) $(OUT_RELEASE)

debug : before_debug $(OUT_DEBUG) after_debug


release : before_release $(OUT_RELEASE) after_release


$(DEBUG_OBJ_DIR)/main.o : main.cpp include/Game/Game.h include/Game/Board.h include/Game/Game_Result.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Random_AI.h include/Players/Outside_Player.h include/Genes/Gene_Pool.h include/Stalemate_Search.h include/Exceptions/Illegal_Move_Exception.h include/Utility.h include/Testing.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c main.cpp -o $(DEBUG_OBJ_DIR)/main.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o : include/Exceptions/Game_Ending_Exception.h include/Game/Color.h src/Exceptions/Game_Ending_Exception.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Exceptions/Game_Ending_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o : include/Exceptions/Illegal_Move_Exception.h src/Exceptions/Illegal_Move_Exception.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Exceptions/Illegal_Move_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o

$(DEBUG_OBJ_DIR)/src/Game/Board.o : include/Game/Board.h include/Moves/Complete_Move.h include/Game/Color.h include/Game/Game_Result.h include/Players/Player.h include/Players/Thinking.h src/Game/Board.cpp include/Game/Clock.h include/Pieces/Pawn.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Queen.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Board.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Board.o

$(DEBUG_OBJ_DIR)/src/Game/Clock.o : include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h src/Game/Clock.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Clock.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Clock.o

$(DEBUG_OBJ_DIR)/src/Game/Color.o : include/Game/Color.h src/Game/Color.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Color.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Color.o

$(DEBUG_OBJ_DIR)/src/Game/Game.o : include/Game/Game.h include/Game/Color.h src/Game/Game.cpp include/Players/Player.h include/Game/Board.h include/Game/Clock.h include/Game/Game_Result.h include/Moves/Complete_Move.h include/Players/Thinking.h include/Exceptions/Game_Ending_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Game.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game.o

$(DEBUG_OBJ_DIR)/src/Game/Game_Result.o : include/Game/Game_Result.h include/Game/Color.h src/Game/Game_Result.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Game_Result.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game_Result.o

$(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Castling_Possible_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Moves/Complete_Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene.o : include/Genes/Gene.h include/Game/Color.h src/Genes/Gene.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o : include/Genes/Gene_Pool.h include/Players/Genetic_AI.h src/Genes/Gene_Pool.cpp include/Game/Game.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Gene_Pool.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o

$(DEBUG_OBJ_DIR)/src/Genes/Genome.o : include/Genes/Genome.h include/Game/Color.h include/Genes/Gene.h src/Genes/Genome.cpp include/Game/Board.h include/Game/Game_Result.h include/Utility.h include/Genes/Total_Force_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Castling_Possible_Gene.h include/Genes/Piece_Strength_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Genome.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : include/Genes/King_Confinement_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/King_Confinement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o : include/Genes/King_Protection_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/King_Protection_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : include/Genes/Look_Ahead_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Look_Ahead_Gene.cpp include/Utility.h include/Game/Board.h include/Game/Clock.h include/Moves/Complete_Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Look_Ahead_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Moves/Complete_Move.h include/Utility.h include/Genes/Piece_Strength_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : include/Genes/Pawn_Advancement_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : include/Genes/Piece_Strength_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Piece_Strength_Gene.cpp include/Utility.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Piece_Strength_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : include/Genes/Sphere_of_Influence_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Moves/Complete_Move.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o : include/Genes/Total_Force_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Genes/Piece_Strength_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Total_Force_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(DEBUG_OBJ_DIR)/src/Moves/Complete_Move.o : include/Moves/Complete_Move.h src/Moves/Complete_Move.cpp include/Moves/Move.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Complete_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Complete_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o : include/Moves/En_Passant.h include/Moves/Pawn_Capture.h src/Moves/En_Passant.cpp include/Moves/Move.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/En_Passant.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o

$(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o : include/Moves/Kingside_Castle.h include/Moves/Move.h src/Moves/Kingside_Castle.cpp include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Kingside_Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o

$(DEBUG_OBJ_DIR)/src/Moves/Move.o : include/Moves/Move.h src/Moves/Move.cpp include/Game/Board.h include/Pieces/Piece.h include/Utility.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o : include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Game/Color.h src/Moves/Pawn_Capture.cpp include/Moves/Move.h include/Pieces/Piece.h include/Game/Board.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Game/Color.h src/Moves/Pawn_Double_Move.cpp include/Moves/Move.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Double_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o : include/Moves/Pawn_Move.h include/Moves/Move.h include/Game/Color.h src/Moves/Pawn_Move.cpp include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o : include/Moves/Pawn_Promotion.h include/Moves/Pawn_Move.h src/Moves/Pawn_Promotion.cpp include/Moves/Move.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Promotion.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Pawn_Promotion.h src/Moves/Pawn_Promotion_by_Capture.cpp include/Pieces/Piece.h include/Game/Board.h include/Moves/Pawn_Capture.h include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o : include/Moves/Queenside_Castle.h include/Moves/Move.h src/Moves/Queenside_Castle.cpp include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Queenside_Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o

$(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o : include/Pieces/Bishop.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Bishop.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Bishop.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o

$(DEBUG_OBJ_DIR)/src/Pieces/King.o : include/Pieces/King.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/King.cpp include/Moves/Move.h include/Moves/Kingside_Castle.h include/Moves/Queenside_Castle.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/King.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/King.o

$(DEBUG_OBJ_DIR)/src/Pieces/Knight.o : include/Pieces/Knight.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Knight.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Knight.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o

$(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o : include/Pieces/Pawn.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Pawn.cpp include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/En_Passant.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/Queen.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Pawn.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o

$(DEBUG_OBJ_DIR)/src/Pieces/Piece.o : include/Pieces/Piece.h include/Moves/Move.h include/Game/Color.h src/Pieces/Piece.cpp include/Utility.h include/Game/Board.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Piece.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o

$(DEBUG_OBJ_DIR)/src/Pieces/Queen.o : include/Pieces/Queen.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Queen.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Queen.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o

$(DEBUG_OBJ_DIR)/src/Pieces/Rook.o : include/Pieces/Rook.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Rook.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Rook.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o

$(DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o : include/Players/Alan_Turing_AI.h include/Players/Player.h src/Players/Alan_Turing_AI.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Alan_Turing_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Alan_Turing_AI.o

$(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o : include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Game/Color.h include/Players/Thinking.h src/Players/CECP_Mediator.cpp include/Players/Player.h include/Moves/Complete_Move.h include/Game/Board.h include/Game/Clock.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Game_Ending_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/CECP_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o

$(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o : include/Players/Claude_Shannon_AI.h include/Players/Player.h include/Game/Color.h src/Players/Claude_Shannon_AI.cpp include/Moves/Complete_Move.h include/Game/Board.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Claude_Shannon_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o : include/Players/Genetic_AI.h include/Players/Player.h include/Moves/Complete_Move.h include/Game/Color.h include/Genes/Genome.h src/Players/Genetic_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Game_Result.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Genetic_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Human_Player.o : include/Players/Human_Player.h include/Players/Player.h src/Players/Human_Player.cpp include/Game/Board.h include/Game/Clock.h include/Moves/Complete_Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Human_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o : include/Players/Outside_Player.h include/Players/Player.h include/Game/Color.h src/Players/Outside_Player.cpp include/Moves/Complete_Move.h include/Game/Board.h include/Players/CECP_Mediator.h include/Players/UCI_Mediator.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Outside_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Player.o : include/Players/Player.h src/Players/Player.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Player.o

$(DEBUG_OBJ_DIR)/src/Players/Random_AI.o : include/Players/Random_AI.h include/Players/Player.h src/Players/Random_AI.cpp include/Game/Board.h include/Moves/Complete_Move.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Random_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o

$(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o : include/Players/UCI_Mediator.h include/Players/Outside_Player.h include/Players/Player.h src/Players/UCI_Mediator.cpp include/Moves/Complete_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Game_Result.h include/Exceptions/Illegal_Move_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/UCI_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o

$(DEBUG_OBJ_DIR)/src/Stalemate_Search.o : include/Stalemate_Search.h src/Stalemate_Search.cpp include/Game/Board.h include/Moves/Complete_Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Stalemate_Search.cpp -o $(DEBUG_OBJ_DIR)/src/Stalemate_Search.o

$(DEBUG_OBJ_DIR)/src/Testing.o : include/Testing.h src/Testing.cpp include/Game/Board.h include/Moves/Complete_Move.h include/Players/Genetic_AI.h include/Game/Clock.h include/Utility.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Testing.cpp -o $(DEBUG_OBJ_DIR)/src/Testing.o

$(DEBUG_OBJ_DIR)/src/Utility.o : include/Utility.h src/Utility.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Utility.cpp -o $(DEBUG_OBJ_DIR)/src/Utility.o

$(OUT_DEBUG) : before_debug $(OBJ_DEBUG)
	$(LD) -o $(OUT_DEBUG) $(OBJ_DEBUG) $(LDFLAGS)

$(OUT_RELEASE) : before_release $(OBJ_RELEASE)
	$(LD) -o $(OUT_RELEASE) $(OBJ_RELEASE) $(LDFLAGS)

$(RELEASE_OBJ_DIR)/main.o : main.cpp include/Game/Game.h include/Game/Board.h include/Game/Game_Result.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Random_AI.h include/Players/Outside_Player.h include/Genes/Gene_Pool.h include/Stalemate_Search.h include/Exceptions/Illegal_Move_Exception.h include/Utility.h include/Testing.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c main.cpp -o $(RELEASE_OBJ_DIR)/main.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o : include/Exceptions/Game_Ending_Exception.h include/Game/Color.h src/Exceptions/Game_Ending_Exception.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Exceptions/Game_Ending_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o : include/Exceptions/Illegal_Move_Exception.h src/Exceptions/Illegal_Move_Exception.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Exceptions/Illegal_Move_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o

$(RELEASE_OBJ_DIR)/src/Game/Board.o : include/Game/Board.h include/Moves/Complete_Move.h include/Game/Color.h include/Game/Game_Result.h include/Players/Player.h include/Players/Thinking.h src/Game/Board.cpp include/Game/Clock.h include/Pieces/Pawn.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Queen.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Board.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Board.o

$(RELEASE_OBJ_DIR)/src/Game/Clock.o : include/Game/Clock.h include/Game/Color.h include/Game/Game_Result.h src/Game/Clock.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Clock.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Clock.o

$(RELEASE_OBJ_DIR)/src/Game/Color.o : include/Game/Color.h src/Game/Color.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Color.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Color.o

$(RELEASE_OBJ_DIR)/src/Game/Game.o : include/Game/Game.h include/Game/Color.h src/Game/Game.cpp include/Players/Player.h include/Game/Board.h include/Game/Clock.h include/Game/Game_Result.h include/Moves/Complete_Move.h include/Players/Thinking.h include/Exceptions/Game_Ending_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Game.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game.o

$(RELEASE_OBJ_DIR)/src/Game/Game_Result.o : include/Game/Game_Result.h include/Game/Color.h src/Game/Game_Result.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Game_Result.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game_Result.o

$(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o : include/Genes/Castling_Possible_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Castling_Possible_Gene.cpp include/Game/Board.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Castling_Possible_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Castling_Possible_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : include/Genes/Freedom_To_Move_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Freedom_To_Move_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Moves/Complete_Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene.o : include/Genes/Gene.h include/Game/Color.h src/Genes/Gene.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o : include/Genes/Gene_Pool.h include/Players/Genetic_AI.h src/Genes/Gene_Pool.cpp include/Game/Game.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Gene_Pool.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o

$(RELEASE_OBJ_DIR)/src/Genes/Genome.o : include/Genes/Genome.h include/Game/Color.h include/Genes/Gene.h src/Genes/Genome.cpp include/Game/Board.h include/Game/Game_Result.h include/Utility.h include/Genes/Total_Force_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h include/Genes/Castling_Possible_Gene.h include/Genes/Piece_Strength_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Genome.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : include/Genes/King_Confinement_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/King_Confinement_Gene.cpp include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/King_Confinement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o : include/Genes/King_Protection_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/King_Protection_Gene.cpp include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/King_Protection_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : include/Genes/Look_Ahead_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Look_Ahead_Gene.cpp include/Utility.h include/Game/Board.h include/Game/Clock.h include/Moves/Complete_Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Look_Ahead_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Moves/Complete_Move.h include/Utility.h include/Genes/Piece_Strength_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : include/Genes/Pawn_Advancement_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Pawn_Advancement_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : include/Genes/Piece_Strength_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Piece_Strength_Gene.cpp include/Utility.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Piece_Strength_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : include/Genes/Sphere_of_Influence_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Sphere_of_Influence_Gene.cpp include/Game/Board.h include/Moves/Complete_Move.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o : include/Genes/Total_Force_Gene.h include/Genes/Gene.h include/Game/Color.h src/Genes/Total_Force_Gene.cpp include/Game/Board.h include/Pieces/Piece.h include/Genes/Piece_Strength_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Total_Force_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(RELEASE_OBJ_DIR)/src/Moves/Complete_Move.o : include/Moves/Complete_Move.h src/Moves/Complete_Move.cpp include/Moves/Move.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Complete_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Complete_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o : include/Moves/En_Passant.h include/Moves/Pawn_Capture.h src/Moves/En_Passant.cpp include/Moves/Move.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/En_Passant.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o

$(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o : include/Moves/Kingside_Castle.h include/Moves/Move.h src/Moves/Kingside_Castle.cpp include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Kingside_Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o

$(RELEASE_OBJ_DIR)/src/Moves/Move.o : include/Moves/Move.h src/Moves/Move.cpp include/Game/Board.h include/Pieces/Piece.h include/Utility.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o : include/Moves/Pawn_Capture.h include/Moves/Pawn_Move.h include/Game/Color.h src/Moves/Pawn_Capture.cpp include/Moves/Move.h include/Pieces/Piece.h include/Game/Board.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Move.h include/Game/Color.h src/Moves/Pawn_Double_Move.cpp include/Moves/Move.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Double_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o : include/Moves/Pawn_Move.h include/Moves/Move.h include/Game/Color.h src/Moves/Pawn_Move.cpp include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o : include/Moves/Pawn_Promotion.h include/Moves/Pawn_Move.h src/Moves/Pawn_Promotion.cpp include/Moves/Move.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Promotion.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : include/Moves/Pawn_Promotion_by_Capture.h include/Moves/Pawn_Promotion.h src/Moves/Pawn_Promotion_by_Capture.cpp include/Pieces/Piece.h include/Game/Board.h include/Moves/Pawn_Capture.h include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o : include/Moves/Queenside_Castle.h include/Moves/Move.h src/Moves/Queenside_Castle.cpp include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Queenside_Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o

$(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o : include/Pieces/Bishop.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Bishop.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Bishop.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o

$(RELEASE_OBJ_DIR)/src/Pieces/King.o : include/Pieces/King.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/King.cpp include/Moves/Move.h include/Moves/Kingside_Castle.h include/Moves/Queenside_Castle.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/King.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/King.o

$(RELEASE_OBJ_DIR)/src/Pieces/Knight.o : include/Pieces/Knight.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Knight.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Knight.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o

$(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o : include/Pieces/Pawn.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Pawn.cpp include/Moves/Move.h include/Moves/Pawn_Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/En_Passant.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/Queen.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Pawn.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o

$(RELEASE_OBJ_DIR)/src/Pieces/Piece.o : include/Pieces/Piece.h include/Moves/Move.h include/Game/Color.h src/Pieces/Piece.cpp include/Utility.h include/Game/Board.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Piece.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o

$(RELEASE_OBJ_DIR)/src/Pieces/Queen.o : include/Pieces/Queen.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Queen.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Queen.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o

$(RELEASE_OBJ_DIR)/src/Pieces/Rook.o : include/Pieces/Rook.h include/Pieces/Piece.h include/Game/Color.h src/Pieces/Rook.cpp include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Rook.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o

$(RELEASE_OBJ_DIR)/src/Players/Alan_Turing_AI.o : include/Players/Alan_Turing_AI.h include/Players/Player.h src/Players/Alan_Turing_AI.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Alan_Turing_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Alan_Turing_AI.o

$(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o : include/Players/CECP_Mediator.h include/Players/Outside_Player.h include/Game/Color.h include/Players/Thinking.h src/Players/CECP_Mediator.cpp include/Players/Player.h include/Moves/Complete_Move.h include/Game/Board.h include/Game/Clock.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Game_Ending_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/CECP_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o

$(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o : include/Players/Claude_Shannon_AI.h include/Players/Player.h include/Game/Color.h src/Players/Claude_Shannon_AI.cpp include/Moves/Complete_Move.h include/Game/Board.h include/Game/Game_Result.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Claude_Shannon_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o : include/Players/Genetic_AI.h include/Players/Player.h include/Moves/Complete_Move.h include/Game/Color.h include/Genes/Genome.h src/Players/Genetic_AI.cpp include/Game/Board.h include/Game/Clock.h include/Game/Game_Result.h include/Players/Thinking.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Genetic_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Human_Player.o : include/Players/Human_Player.h include/Players/Player.h src/Players/Human_Player.cpp include/Game/Board.h include/Game/Clock.h include/Moves/Complete_Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Human_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o : include/Players/Outside_Player.h include/Players/Player.h include/Game/Color.h src/Players/Outside_Player.cpp include/Moves/Complete_Move.h include/Game/Board.h include/Players/CECP_Mediator.h include/Players/UCI_Mediator.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Outside_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Player.o : include/Players/Player.h src/Players/Player.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Player.o

$(RELEASE_OBJ_DIR)/src/Players/Random_AI.o : include/Players/Random_AI.h include/Players/Player.h src/Players/Random_AI.cpp include/Game/Board.h include/Moves/Complete_Move.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Random_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o

$(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o : include/Players/UCI_Mediator.h include/Players/Outside_Player.h include/Players/Player.h src/Players/UCI_Mediator.cpp include/Moves/Complete_Move.h include/Game/Board.h include/Game/Clock.h include/Game/Game_Result.h include/Exceptions/Illegal_Move_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/UCI_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o

$(RELEASE_OBJ_DIR)/src/Stalemate_Search.o : include/Stalemate_Search.h src/Stalemate_Search.cpp include/Game/Board.h include/Moves/Complete_Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Stalemate_Search.cpp -o $(RELEASE_OBJ_DIR)/src/Stalemate_Search.o

$(RELEASE_OBJ_DIR)/src/Testing.o : include/Testing.h src/Testing.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Testing.cpp -o $(RELEASE_OBJ_DIR)/src/Testing.o

$(RELEASE_OBJ_DIR)/src/Utility.o : include/Utility.h src/Utility.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Utility.cpp -o $(RELEASE_OBJ_DIR)/src/Utility.o

.PHONY : all after_debug after_release before_debug before_release clean clean_debug clean_release debug release


