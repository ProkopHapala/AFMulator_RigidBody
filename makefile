
#FLAGS = -std=c++11 -w
#FLAGS = -std=c++11 -O0 -g -w
#FLAGS = -std=c++11 -Og -g -Wall
#FLAGS = -std=c++11 -O3 -w
FLAGS = -std=c++11 -Ofast -w
#FLAGS = -std=c++11 -O3 -ftree-vectorize -unroll-loops -w
#FLAGS = -std=c++11 -O3 -ftree-vectorize -unroll-loops -ffast-math -w
#FLAGS = -std=c++11 -O3 -ftree-vectorize -unroll-loops --param max-unroll-times=4 -ffast-math -march=native -mtune=native -mssse3 -w
LINKFLAGS = -Wl,-rpath -Wl,LIBDIR
SRC = src/main.cpp

BIN = program.x

all: clean program

program:
	g++ $(FLAGS) $(SRC) -o $(BIN) -lm -lSDL2 -lSDL2_ttf -lGL -lGLU $(LINKFLAGS)

clean:
	rm -rf *.o $(BIN)  

