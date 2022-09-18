#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS =  -I./src/ravencode 

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs

	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj
RDIR = ../ravencode/pub/src

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_chess = \
	$(ODIR)/main.o \
	$(ODIR)/cChess.o

### COMPILE

$(ODIR)/%.o: %.cpp cChess.h
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

starter: $(OBJ_chess) 
	g++ $(OPT) \
	-o bin/chess.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

