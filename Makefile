CXX = g++
LD = g++

CFLAGS = -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wzero-as-null-pointer-constant -Wmain -pedantic -Wextra -Wall -std=c++11 -fexceptions -Iinclude
LDFLAGS = -pthread

OUT_RELEASE = bin/release/genetic_chess
RELEASE_OBJ_DIR = obj/release
OBJ_RELEASE = $(RELEASE_OBJ_DIR)/main.o $(RELEASE_OBJ_DIR)/src/Exceptions/Checkmate_Exception.o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o $(RELEASE_OBJ_DIR)/src/Exceptions/Out_Of_Time_Exception.o $(RELEASE_OBJ_DIR)/src/Exceptions/Stalemate_Exception.o $(RELEASE_OBJ_DIR)/src/Game/Board.o $(RELEASE_OBJ_DIR)/src/Game/Clock.o $(RELEASE_OBJ_DIR)/src/Game/Color.o $(RELEASE_OBJ_DIR)/src/Game/Game.o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o $(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o $(RELEASE_OBJ_DIR)/src/Moves/Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o $(RELEASE_OBJ_DIR)/src/Pieces/King.o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o $(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o $(RELEASE_OBJ_DIR)/src/Players/Communication_Mediator.o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o $(RELEASE_OBJ_DIR)/src/Players/Player.o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o $(RELEASE_OBJ_DIR)/src/Testing.o $(RELEASE_OBJ_DIR)/src/Utility.o 
CFLAGS_RELEASE = -s -fexpensive-optimizations -O3


OUT_DEBUG = bin/debug/genetic_chess
DEBUG_OBJ_DIR = obj/debug
OBJ_DEBUG = $(DEBUG_OBJ_DIR)/main.o $(DEBUG_OBJ_DIR)/src/Exceptions/Checkmate_Exception.o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o $(DEBUG_OBJ_DIR)/src/Exceptions/Out_Of_Time_Exception.o $(DEBUG_OBJ_DIR)/src/Exceptions/Stalemate_Exception.o $(DEBUG_OBJ_DIR)/src/Game/Board.o $(DEBUG_OBJ_DIR)/src/Game/Clock.o $(DEBUG_OBJ_DIR)/src/Game/Color.o $(DEBUG_OBJ_DIR)/src/Game/Game.o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o $(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o $(DEBUG_OBJ_DIR)/src/Moves/Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o $(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o $(DEBUG_OBJ_DIR)/src/Pieces/King.o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o $(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o $(DEBUG_OBJ_DIR)/src/Players/Communication_Mediator.o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o $(DEBUG_OBJ_DIR)/src/Players/Player.o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o $(DEBUG_OBJ_DIR)/src/Testing.o $(DEBUG_OBJ_DIR)/src/Utility.o 
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


$(DEBUG_OBJ_DIR)/main.o : main.cpp include/Game/Game.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Random_AI.h include/Players/Outside_Player.h include/Genes/Gene_Pool.h include/Testing.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c main.cpp -o $(DEBUG_OBJ_DIR)/main.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Checkmate_Exception.o : src/Exceptions/Checkmate_Exception.cpp include/Exceptions/Checkmate_Exception.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Exceptions/Checkmate_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Checkmate_Exception.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o : src/Exceptions/Game_Ending_Exception.cpp include/Exceptions/Game_Ending_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Exceptions/Game_Ending_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o : src/Exceptions/Illegal_Move_Exception.cpp include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Exceptions/Illegal_Move_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Out_Of_Time_Exception.o : src/Exceptions/Out_Of_Time_Exception.cpp include/Exceptions/Out_Of_Time_Exception.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Exceptions/Out_Of_Time_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Out_Of_Time_Exception.o

$(DEBUG_OBJ_DIR)/src/Exceptions/Stalemate_Exception.o : src/Exceptions/Stalemate_Exception.cpp include/Exceptions/Stalemate_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Exceptions/Stalemate_Exception.cpp -o $(DEBUG_OBJ_DIR)/src/Exceptions/Stalemate_Exception.o

$(DEBUG_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Game/Board.h include/Pieces/Pawn.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Queen.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Checkmate_Exception.h include/Exceptions/Stalemate_Exception.h include/Exceptions/Promotion_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Board.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Board.o

$(DEBUG_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Exceptions/Out_Of_Time_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Clock.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Clock.o

$(DEBUG_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Color.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Color.o

$(DEBUG_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Game/Game.h include/Players/Player.h include/Game/Board.h include/Game/Clock.h include/Moves/Move.h include/Exceptions/Game_Ending_Exception.h include/Exceptions/Out_Of_Time_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Game/Game.cpp -o $(DEBUG_OBJ_DIR)/src/Game/Game.o

$(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Genes/Freedom_To_Move_Gene.h include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Genes/Gene.h include/Game/Color.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o : src/Genes/Gene_Pool.cpp include/Genes/Gene_Pool.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Game/Game.h include/Game/Board.h include/Exceptions/End_Of_File_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Gene_Pool.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Gene_Pool.o

$(DEBUG_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Genes/Genome.h include/Game/Board.h include/Game/Color.h include/Utility.h include/Genes/Gene.h include/Genes/Total_Force_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Genome.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Genome.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Genes/King_Confinement_Gene.h include/Game/Board.h include/Game/Color.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/King_Confinement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Genes/King_Protection_Gene.h include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/King_Protection_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Genes/Look_Ahead_Gene.h include/Utility.h include/Game/Board.h include/Game/Clock.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Look_Ahead_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Genes/Pawn_Advancement_Gene.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Genes/Piece_Strength_Gene.h include/Utility.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Piece_Strength_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Genes/Sphere_of_Influence_Gene.h include/Game/Board.h include/Moves/Move.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Genes/Total_Force_Gene.h include/Game/Board.h include/Pieces/Piece.h include/Genes/Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Genes/Total_Force_Gene.cpp -o $(DEBUG_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Moves/En_Passant.h include/Moves/Pawn_Capture.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/En_Passant.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/En_Passant.o

$(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o : src/Moves/Kingside_Castle.cpp include/Moves/Kingside_Castle.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Kingside_Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Kingside_Castle.o

$(DEBUG_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Moves/Move.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Moves/Pawn_Capture.h include/Pieces/Piece.h include/Game/Board.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Moves/Pawn_Double_Move.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Double_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Moves/Pawn_Move.h include/Moves/Move.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Move.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Move.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Moves/Pawn_Promotion.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Promotion.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Moves/Pawn_Promotion_by_Capture.h include/Pieces/Piece.h include/Game/Board.h include/Moves/Pawn_Capture.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o : src/Moves/Queenside_Castle.cpp include/Moves/Queenside_Castle.h include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Moves/Queenside_Castle.cpp -o $(DEBUG_OBJ_DIR)/src/Moves/Queenside_Castle.o

$(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o : src/Pieces/Bishop.cpp include/Pieces/Bishop.h include/Pieces/Piece.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Bishop.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Bishop.o

$(DEBUG_OBJ_DIR)/src/Pieces/King.o : src/Pieces/King.cpp include/Pieces/King.h include/Moves/Kingside_Castle.h include/Moves/Queenside_Castle.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/King.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/King.o

$(DEBUG_OBJ_DIR)/src/Pieces/Knight.o : src/Pieces/Knight.cpp include/Pieces/Knight.h include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Knight.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Knight.o

$(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o : src/Pieces/Pawn.cpp include/Pieces/Pawn.h include/Moves/Pawn_Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/En_Passant.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/Queen.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Pawn.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Pawn.o

$(DEBUG_OBJ_DIR)/src/Pieces/Piece.o : src/Pieces/Piece.cpp include/Pieces/Piece.h include/Utility.h include/Game/Board.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Piece.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Piece.o

$(DEBUG_OBJ_DIR)/src/Pieces/Queen.o : src/Pieces/Queen.cpp include/Pieces/Queen.h include/Pieces/Piece.h include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Queen.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Queen.o

$(DEBUG_OBJ_DIR)/src/Pieces/Rook.o : src/Pieces/Rook.cpp include/Pieces/Rook.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Pieces/Rook.cpp -o $(DEBUG_OBJ_DIR)/src/Pieces/Rook.o

$(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Players/CECP_Mediator.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/CECP_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/CECP_Mediator.o

$(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o : src/Players/Claude_Shannon_AI.cpp include/Players/Claude_Shannon_AI.h include/Moves/Move.h include/Game/Board.h include/Game/Color.h include/Exceptions/Checkmate_Exception.h include/Exceptions/Game_Ending_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Claude_Shannon_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Claude_Shannon_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Communication_Mediator.o : src/Players/Communication_Mediator.cpp include/Players/Communication_Mediator.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Communication_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Communication_Mediator.o

$(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Players/Genetic_AI.h include/Utility.h include/Moves/Move.h include/Game/Board.h include/Game/Clock.h include/Pieces/Piece.h include/Exceptions/Checkmate_Exception.h include/Exceptions/Game_Ending_Exception.h include/Exceptions/End_Of_File_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Genetic_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Genetic_AI.o

$(DEBUG_OBJ_DIR)/src/Players/Human_Player.o : src/Players/Human_Player.cpp include/Players/Human_Player.h include/Game/Board.h include/Game/Clock.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Human_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Human_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o : src/Players/Outside_Player.cpp include/Players/Outside_Player.h include/Moves/Move.h include/Game/Board.h include/Game/Color.h include/Exceptions/Game_Ending_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Outside_Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Outside_Player.o

$(DEBUG_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h include/Game/Color.h include/Exceptions/Game_Ending_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Player.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Player.o

$(DEBUG_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Players/Random_AI.h include/Game/Board.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/Random_AI.cpp -o $(DEBUG_OBJ_DIR)/src/Players/Random_AI.o

$(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o : src/Players/UCI_Mediator.cpp include/Players/UCI_Mediator.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Players/UCI_Mediator.cpp -o $(DEBUG_OBJ_DIR)/src/Players/UCI_Mediator.o

$(DEBUG_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Testing.h include/Game/Board.h include/Moves/Move.h include/Players/Genetic_AI.h include/Utility.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Testing.cpp -o $(DEBUG_OBJ_DIR)/src/Testing.o

$(DEBUG_OBJ_DIR)/src/Utility.o : src/Utility.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_DEBUG) -c src/Utility.cpp -o $(DEBUG_OBJ_DIR)/src/Utility.o

$(OUT_DEBUG) : before_debug $(OBJ_DEBUG)
	$(LD) -o $(OUT_DEBUG) $(OBJ_DEBUG) $(LDFLAGS)

$(OUT_RELEASE) : before_release $(OBJ_RELEASE)
	$(LD) -o $(OUT_RELEASE) $(OBJ_RELEASE) $(LDFLAGS)

$(RELEASE_OBJ_DIR)/main.o : main.cpp include/Game/Game.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Players/Random_AI.h include/Players/Outside_Player.h include/Genes/Gene_Pool.h include/Testing.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c main.cpp -o $(RELEASE_OBJ_DIR)/main.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Checkmate_Exception.o : src/Exceptions/Checkmate_Exception.cpp include/Exceptions/Checkmate_Exception.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Exceptions/Checkmate_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Checkmate_Exception.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o : src/Exceptions/Game_Ending_Exception.cpp include/Exceptions/Game_Ending_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Exceptions/Game_Ending_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Game_Ending_Exception.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o : src/Exceptions/Illegal_Move_Exception.cpp include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Exceptions/Illegal_Move_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Illegal_Move_Exception.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Out_Of_Time_Exception.o : src/Exceptions/Out_Of_Time_Exception.cpp include/Exceptions/Out_Of_Time_Exception.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Exceptions/Out_Of_Time_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Out_Of_Time_Exception.o

$(RELEASE_OBJ_DIR)/src/Exceptions/Stalemate_Exception.o : src/Exceptions/Stalemate_Exception.cpp include/Exceptions/Stalemate_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Exceptions/Stalemate_Exception.cpp -o $(RELEASE_OBJ_DIR)/src/Exceptions/Stalemate_Exception.o

$(RELEASE_OBJ_DIR)/src/Game/Board.o : src/Game/Board.cpp include/Game/Board.h include/Pieces/Pawn.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/King.h include/Pieces/Queen.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Checkmate_Exception.h include/Exceptions/Stalemate_Exception.h include/Exceptions/Promotion_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Board.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Board.o

$(RELEASE_OBJ_DIR)/src/Game/Clock.o : src/Game/Clock.cpp include/Game/Clock.h include/Exceptions/Out_Of_Time_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Clock.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Clock.o

$(RELEASE_OBJ_DIR)/src/Game/Color.o : src/Game/Color.cpp include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Color.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Color.o

$(RELEASE_OBJ_DIR)/src/Game/Game.o : src/Game/Game.cpp include/Game/Game.h include/Players/Player.h include/Game/Board.h include/Game/Clock.h include/Moves/Move.h include/Exceptions/Game_Ending_Exception.h include/Exceptions/Out_Of_Time_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Game/Game.cpp -o $(RELEASE_OBJ_DIR)/src/Game/Game.o

$(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o : src/Genes/Freedom_To_Move_Gene.cpp include/Genes/Freedom_To_Move_Gene.h include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Freedom_To_Move_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Freedom_To_Move_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene.o : src/Genes/Gene.cpp include/Genes/Gene.h include/Game/Color.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o : src/Genes/Gene_Pool.cpp include/Genes/Gene_Pool.h include/Players/Genetic_AI.h include/Players/Human_Player.h include/Game/Game.h include/Game/Board.h include/Exceptions/End_Of_File_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Gene_Pool.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Gene_Pool.o

$(RELEASE_OBJ_DIR)/src/Genes/Genome.o : src/Genes/Genome.cpp include/Genes/Genome.h include/Game/Board.h include/Game/Color.h include/Utility.h include/Genes/Gene.h include/Genes/Total_Force_Gene.h include/Genes/Freedom_To_Move_Gene.h include/Genes/Pawn_Advancement_Gene.h include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Sphere_of_Influence_Gene.h include/Genes/Look_Ahead_Gene.h include/Genes/King_Confinement_Gene.h include/Genes/King_Protection_Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Genome.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Genome.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o : src/Genes/King_Confinement_Gene.cpp include/Genes/King_Confinement_Gene.h include/Game/Board.h include/Game/Color.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/King_Confinement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Confinement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o : src/Genes/King_Protection_Gene.cpp include/Genes/King_Protection_Gene.h include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/King_Protection_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/King_Protection_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o : src/Genes/Look_Ahead_Gene.cpp include/Genes/Look_Ahead_Gene.h include/Utility.h include/Game/Board.h include/Game/Clock.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Look_Ahead_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Look_Ahead_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o : src/Genes/Opponent_Pieces_Targeted_Gene.cpp include/Genes/Opponent_Pieces_Targeted_Gene.h include/Genes/Piece_Strength_Gene.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Opponent_Pieces_Targeted_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Opponent_Pieces_Targeted_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o : src/Genes/Pawn_Advancement_Gene.cpp include/Genes/Pawn_Advancement_Gene.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Pawn_Advancement_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Pawn_Advancement_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o : src/Genes/Piece_Strength_Gene.cpp include/Genes/Piece_Strength_Gene.h include/Utility.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Piece_Strength_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Piece_Strength_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o : src/Genes/Sphere_of_Influence_Gene.cpp include/Genes/Sphere_of_Influence_Gene.h include/Game/Board.h include/Moves/Move.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Sphere_of_Influence_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Sphere_of_Influence_Gene.o

$(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o : src/Genes/Total_Force_Gene.cpp include/Genes/Total_Force_Gene.h include/Game/Board.h include/Pieces/Piece.h include/Genes/Gene.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Genes/Total_Force_Gene.cpp -o $(RELEASE_OBJ_DIR)/src/Genes/Total_Force_Gene.o

$(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o : src/Moves/En_Passant.cpp include/Moves/En_Passant.h include/Moves/Pawn_Capture.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/En_Passant.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/En_Passant.o

$(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o : src/Moves/Kingside_Castle.cpp include/Moves/Kingside_Castle.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Kingside_Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Kingside_Castle.o

$(RELEASE_OBJ_DIR)/src/Moves/Move.o : src/Moves/Move.cpp include/Moves/Move.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o : src/Moves/Pawn_Capture.cpp include/Moves/Pawn_Capture.h include/Pieces/Piece.h include/Game/Board.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o : src/Moves/Pawn_Double_Move.cpp include/Moves/Pawn_Double_Move.h include/Pieces/Piece.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Double_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Double_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o : src/Moves/Pawn_Move.cpp include/Moves/Pawn_Move.h include/Moves/Move.h include/Game/Board.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Move.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Move.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o : src/Moves/Pawn_Promotion.cpp include/Moves/Pawn_Promotion.h include/Game/Board.h include/Pieces/Piece.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Promotion.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion.o

$(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o : src/Moves/Pawn_Promotion_by_Capture.cpp include/Moves/Pawn_Promotion_by_Capture.h include/Pieces/Piece.h include/Game/Board.h include/Moves/Pawn_Capture.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Pawn_Promotion_by_Capture.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Pawn_Promotion_by_Capture.o

$(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o : src/Moves/Queenside_Castle.cpp include/Moves/Queenside_Castle.h include/Game/Board.h include/Pieces/Piece.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Moves/Queenside_Castle.cpp -o $(RELEASE_OBJ_DIR)/src/Moves/Queenside_Castle.o

$(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o : src/Pieces/Bishop.cpp include/Pieces/Bishop.h include/Pieces/Piece.h include/Game/Color.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Bishop.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Bishop.o

$(RELEASE_OBJ_DIR)/src/Pieces/King.o : src/Pieces/King.cpp include/Pieces/King.h include/Moves/Kingside_Castle.h include/Moves/Queenside_Castle.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/King.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/King.o

$(RELEASE_OBJ_DIR)/src/Pieces/Knight.o : src/Pieces/Knight.cpp include/Pieces/Knight.h include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Knight.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Knight.o

$(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o : src/Pieces/Pawn.cpp include/Pieces/Pawn.h include/Moves/Pawn_Move.h include/Moves/Pawn_Capture.h include/Moves/Pawn_Double_Move.h include/Moves/Pawn_Promotion.h include/Moves/Pawn_Promotion_by_Capture.h include/Moves/En_Passant.h include/Pieces/Rook.h include/Pieces/Knight.h include/Pieces/Bishop.h include/Pieces/Queen.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Pawn.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Pawn.o

$(RELEASE_OBJ_DIR)/src/Pieces/Piece.o : src/Pieces/Piece.cpp include/Pieces/Piece.h include/Utility.h include/Game/Board.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Piece.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Piece.o

$(RELEASE_OBJ_DIR)/src/Pieces/Queen.o : src/Pieces/Queen.cpp include/Pieces/Queen.h include/Pieces/Piece.h include/Moves/Move.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Queen.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Queen.o

$(RELEASE_OBJ_DIR)/src/Pieces/Rook.o : src/Pieces/Rook.cpp include/Pieces/Rook.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Pieces/Rook.cpp -o $(RELEASE_OBJ_DIR)/src/Pieces/Rook.o

$(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o : src/Players/CECP_Mediator.cpp include/Players/CECP_Mediator.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/CECP_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/CECP_Mediator.o

$(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o : src/Players/Claude_Shannon_AI.cpp include/Players/Claude_Shannon_AI.h include/Moves/Move.h include/Game/Board.h include/Game/Color.h include/Exceptions/Checkmate_Exception.h include/Exceptions/Game_Ending_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Claude_Shannon_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Claude_Shannon_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Communication_Mediator.o : src/Players/Communication_Mediator.cpp include/Players/Communication_Mediator.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Communication_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Communication_Mediator.o

$(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o : src/Players/Genetic_AI.cpp include/Players/Genetic_AI.h include/Utility.h include/Moves/Move.h include/Game/Board.h include/Game/Clock.h include/Pieces/Piece.h include/Exceptions/Checkmate_Exception.h include/Exceptions/Game_Ending_Exception.h include/Exceptions/End_Of_File_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Genetic_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Genetic_AI.o

$(RELEASE_OBJ_DIR)/src/Players/Human_Player.o : src/Players/Human_Player.cpp include/Players/Human_Player.h include/Game/Board.h include/Game/Clock.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Exceptions/Promotion_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Human_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Human_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o : src/Players/Outside_Player.cpp include/Players/Outside_Player.h include/Moves/Move.h include/Game/Board.h include/Game/Color.h include/Exceptions/Game_Ending_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Outside_Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Outside_Player.o

$(RELEASE_OBJ_DIR)/src/Players/Player.o : src/Players/Player.cpp include/Players/Player.h include/Game/Color.h include/Exceptions/Game_Ending_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Player.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Player.o

$(RELEASE_OBJ_DIR)/src/Players/Random_AI.o : src/Players/Random_AI.cpp include/Players/Random_AI.h include/Game/Board.h include/Moves/Move.h include/Exceptions/Illegal_Move_Exception.h include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/Random_AI.cpp -o $(RELEASE_OBJ_DIR)/src/Players/Random_AI.o

$(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o : src/Players/UCI_Mediator.cpp include/Players/UCI_Mediator.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Players/UCI_Mediator.cpp -o $(RELEASE_OBJ_DIR)/src/Players/UCI_Mediator.o

$(RELEASE_OBJ_DIR)/src/Testing.o : src/Testing.cpp include/Testing.h include/Game/Board.h include/Moves/Move.h include/Players/Genetic_AI.h include/Utility.h include/Exceptions/Illegal_Move_Exception.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Testing.cpp -o $(RELEASE_OBJ_DIR)/src/Testing.o

$(RELEASE_OBJ_DIR)/src/Utility.o : src/Utility.cpp include/Utility.h
	$(CXX) $(CFLAGS) $(CFLAGS_RELEASE) -c src/Utility.cpp -o $(RELEASE_OBJ_DIR)/src/Utility.o

.PHONY : all after_debug after_release before_debug before_release clean clean_debug clean_release debug release


