
struct Material;
struct Texture;

typedef struct Part {
	string n;
	int v;
	string s;
	float r1;
	float r2;
	Material *m;
	Texture *t;
} Part;
typedef map<string, Part>::iterator it_Part;


