CFLAGS=-c -Wall -I./include -I../shared/include -O3 -std=c++11

LFLAGS=-L./lib -lGL -lGLU -llua -lchipmunk -lrt

CC=clang++

all: pinball

pinball: main.o PinballHost.o
	$(CC) main.o PinballBridge.o Playfield.o Physics.o Camera.o PinballHost.o Util.o Editor.o XEngine.o Renderer.o glfont2.o -o pinball $(LFLAGS) 	

SRC=./src/

main.o: $(SRC)main.cpp PinballHost.o
	$(CC) $(CFLAGS) $(SRC)main.cpp 

PinballHost.o: PinballBridge.o Playfield.o Physics.o Camera.o Editor.o XEngine.o Renderer.o $(SRC)PinballHost.cpp
	$(CC) $(CFLAGS) $(SRC)PinballHost.cpp

PinballBridge.o: $(SRC)PinballBridge.cpp
	$(CC) $(CFLAGS) $(SRC)PinballBridge.cpp

XEngine.o: $(SRC)XEngine.cpp
	$(CC) $(CFLAGS) $(SRC)XEngine.cpp

S_SRC=../shared/src/

Playfield.o: $(S_SRC)Playfield.cpp
	$(CC) $(CFLAGS) $(S_SRC)Playfield.cpp

Physics.o: Util.o $(S_SRC)Physics.cpp
	$(CC) $(CFLAGS) $(S_SRC)Physics.cpp

Camera.o: $(S_SRC)Camera.cpp
	$(CC) $(CFLAGS) $(S_SRC)Camera.cpp

Util.o: $(S_SRC)Util.cpp
	$(CC) $(CFLAGS) $(S_SRC)Util.cpp

Editor.o: $(S_SRC)Editor.cpp
	$(CC) $(CFLAGS) $(S_SRC)Editor.cpp

Renderer.o: glfont2.o $(S_SRC)Renderer.cpp
	$(CC) $(CFLAGS) $(S_SRC)Renderer.cpp

glfont2.o: $(S_SRC)glfont2.cpp
	$(CC) $(CFLAGS) $(S_SRC)glfont2.cpp

clean:
	rm -rf *.o *.s *.bc *.native pinball
