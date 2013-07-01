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
	const char *getTexturePath(const char *texturePath);
    	const HostProperties *getHostProperties();
	GLTexture *createRGBATexture(const char *textureName);
	void playSound(const char *soundName);
	void addTimer(float duration, int id, const ITimerDelegate *timerDelegate);
private:
	const char *_gameName;
	const char *_basePath;
	const char *_scriptPath;
	const char *_texturePath;
	const HostProperties *_hostProperties;
};
