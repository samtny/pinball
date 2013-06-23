CFLAGS=-c -Wall -I ../shared/include

LFLAGS=-lGL -lGLU

CC=clang++

all: pinball

pinball: main.o PinballHost.o
	$(CC) $(LFLAGS) main.o PinballHost.o -o pinball 	

main.o: main.cpp PinballHost.o
	$(CC) $(CFLAGS) main.cpp 

PinballHost.o: PinballHost.cpp
	$(CC) $(CFLAGS) PinballHost.cpp

clean:
	rm -rf *.o *.s *.bc *.native pinball		
