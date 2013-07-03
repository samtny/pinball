#include "PinballHost.h"

int main(void) {
	PinballHost *host = new PinballHost();
	
	host->init("Cosmos");
	host->start();

	return 0;
}
