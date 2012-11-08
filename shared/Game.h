
class PinballNativeImpl;

class Game {
public:
	Game(void);
	~Game(void);
	void start(void);
private:
	PinballNativeImpl *_pinballNative;
	bool finished;
	bool paused;
};

