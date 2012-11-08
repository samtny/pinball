//
//  PinballNative.m
//  PorkyPinball
//
//  Created by Richard Thompson on 11/8/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import "PinballNative.h"

@implementation PinballNative

PinballNativeImpl::PinballNativeImpl(void) : self (NULL) {
    
}

PinballNativeImpl::~PinballNativeImpl(void) {
    [(id)self dealloc];
}

bool PinballNativeImpl::init(void) {
    self = [[PinballNative alloc] init];
    return YES;
}

const char * PinballNativeImpl::getPathForScriptFileName(void * scriptFileName) {
    return [(id)self getPathForScriptFileName:scriptFileName];
}

DisplayProperties * PinballNativeImpl::getDisplayProperties() {
    return [(id)self getDisplayProperties];
}

void PinballNativeImpl::playSound(void * soundName) {
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
    return props;
}

-(void)playSound:(void *)soundName {
    // TODO: something;
}

@end
