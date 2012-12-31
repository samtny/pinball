
using namespace std;
#include <string>
#include <vector>
#include <map>

struct Material;
struct Texture;

typedef struct LayoutItem {
	string n;
	int v;
	string s;
	float r1;
	float r2;
	Material *m;
	Texture *t;
} Part;
typedef map<string, LayoutItem>::iterator it_LayoutItem;

