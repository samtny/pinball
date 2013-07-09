#include "PinballHost.h"
#include <iostream>

int main(int argc, const char **argv) {
	
	PinballHost *host = new PinballHost();

	if (argc > 1) {
		host->init(argv[1]);
	} else {
		host->init("CatchAThief");
	}
	
	host->start();

	return 0;
}
