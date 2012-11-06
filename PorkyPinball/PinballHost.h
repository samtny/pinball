
// TODO: use this same header for multi-platform via compiler flags?

#ifndef __PINBALLHOST_C_INTERFACE_H__
#define __PINBALLHOST_C_INTERFACE_H__

class PinballHost
{
public:
    PinballHost (void);
    ~PinballHost(void);

    bool init(void);
    char *  getPathForScriptFileName(void * scriptFileName);
    void playSound(char * soundName);

private:
    void * self;
};

#endif

