//
//  PinballNative.m
//  PorkyPinball
//
//  Created by Richard Thompson on 11/8/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import "PinballBridge.h"

@implementation PinballBridge

PinballBridgeInterface::PinballBridgeInterface(void) : self (NULL) {
    
}

PinballBridgeInterface::~PinballBridgeInterface(void) {
    [(id)self dealloc];
}

bool PinballBridgeInterface::init(void) {
    self = [[PinballBridge alloc] init];
    [(PinballBridge *)self initI];
    return YES;
}

-(void)initI {
    
}

const char * PinballBridgeInterface::getPathForScriptFileName(void * scriptFileName) {
    return [(id)self getPathForScriptFileName:scriptFileName];
}

DisplayProperties * PinballBridgeInterface::getDisplayProperties() {
    return [(id)self getDisplayProperties];
}

void PinballBridgeInterface::playSound(void * soundName) {
    [(id)self playSound:soundName];
}

-(const char *)getPathForScriptFileName:(void *)scriptFileName {
    NSString *prefix = [[NSString stringWithUTF8String:(const char *)scriptFileName] stringByDeletingPathExtension];
    NSString *suffix = [[NSString stringWithUTF8String:(const char *)scriptFileName] pathExtension];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:prefix ofType:suffix];
    const char *path = [filePath UTF8String];
    return path;
}

-(DisplayProperties *)getDisplayProperties {
    DisplayProperties *props = new DisplayProperties();
    // TODO: vary by view props;
    props->viewportX = 0;
    props->viewportY = 0;
    props->viewportHeight = 480;
    props->viewportWidth = 320;
    return props;
}

-(void)playSound:(void *)soundName {
    // TODO: something;
}

@end
