#include "PinballHost.h"

int main(void) {
	PinballHost *host = new PinballHost();
	
	host->start("Cosmos");

	return 0;
}
