#include "PinballBridgeInterface.h"

class PinballBridge
{
public:
	PinballBridge(void);
	~PinballBridge(void);
	void setGameName(const char *gameName);
	const char *getGameName();
	const char *getBasePath();
	const char *getScriptPath(const char *scriptName);
    	const HostProperties *getHostProperties();
	void playSound(const char *soundName);
	void addTimer(float duration, int id, const ITimerDelegate *timerDelegate);
private:
	const char *_gameName;
	const char *_basePath;
	const char *_scriptPath;
};
