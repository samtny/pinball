#include "PinballHost.h"

int main(void) {
	PinballHost *host = new PinballHost();
	host->init();
	host->start();

	return 0;
}
