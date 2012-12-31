
using namespace std;
#include <string>
#include <map>

typedef struct Material {
	string n;
	float e;
	float f;
	float d;
} Material;
typedef map<string, Material>::iterator it_Material;

