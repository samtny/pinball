CFLAGS=-c -Wall -I./include -I../shared/include -O3 -std=c++11

LFLAGS=-L./lib -lGL -lGLU -llua -lchipmunk -lrt

CC=clang++

all: pinball

pinball: main.o PinballHost.o
	$(CC) main.o PinballBridge.o Playfield.o Physics.o Camera.o PinballHost.o Util.o -o pinball $(LFLAGS) 	

SRC=./src/

main.o: $(SRC)main.cpp PinballHost.o
	$(CC) $(CFLAGS) $(SRC)main.cpp 

PinballHost.o: PinballBridge.o Playfield.o Physics.o Camera.o $(SRC)PinballHost.cpp
	$(CC) $(CFLAGS) $(SRC)PinballHost.cpp

PinballBridge.o: $(SRC)PinballBridge.cpp
	$(CC) $(CFLAGS) $(SRC)PinballBridge.cpp

S_SRC=../shared/src/

Playfield.o: $(S_SRC)Playfield.cpp
	$(CC) $(CFLAGS) $(S_SRC)Playfield.cpp

Physics.o: Util.o $(S_SRC)Physics.cpp
	$(CC) $(CFLAGS) $(S_SRC)Physics.cpp

Camera.o: $(S_SRC)Camera.cpp
	$(CC) $(CFLAGS) $(S_SRC)Camera.cpp

Util.o: $(S_SRC)Util.cpp
	$(CC) $(CFLAGS) $(S_SRC)Util.cpp

clean:
	rm -rf *.o *.s *.bc *.native pinball		
