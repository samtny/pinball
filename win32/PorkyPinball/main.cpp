
#include "PinballHost.h"

int main(void) {
	PinballHost *host = new PinballHost();
	host->start("PorkyPinball");
	return 0;
}

