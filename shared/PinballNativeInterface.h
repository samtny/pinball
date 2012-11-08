
#ifndef __PINBALLNATIVE_C_INTERFACE_H__
#define __PINBALLNATIVE_C_INTERFACE_H__

typedef struct {
	int viewHeight;
	int viewWidth;
} DisplayProperties;

class PinballNativeImpl
{
public:
    
	PinballNativeImpl(void);
    ~PinballNativeImpl(void);
    
#ifdef _WIN32
	void init(void);
#elif __APPLE__
	bool init(void);
#endif

	const char * getPathForScriptFileName(void * scriptFileName);

    DisplayProperties *getDisplayProperties();
	
	void playSound(void * soundName);

private:
#ifdef _WIN32
	void *_this;
#elif __APPLE__
    void *self;
#endif
};

#endif

