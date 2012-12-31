
using namespace std;
#include <string>;
#include <stdint.h>;

typedef struct Texture {
	string n;
	int x;
	int y;
	string filename;
	uint32_t gl_index;
	int w;
	int h;
	float a;
} Texture;
