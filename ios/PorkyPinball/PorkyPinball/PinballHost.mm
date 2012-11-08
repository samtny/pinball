//
//  PinballHost.m
//  PorkyPinball
//
//  Created by Richard Thompson on 11/7/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import "PinballHost.h"

#import "PinballHostInterface.h"

#import "Physics.h"

#import "Renderer.h"

@implementation PinballHost

PinballHostImpl::PinballHostImpl(void) : self(NULL) {
    
}

PinballHostImpl::~PinballHostImpl(void) {
    [(id)self dealloc];
}



const char *PinballHostImpl::getPathForScriptFileName(void *scriptFileName) {
    return [(id)self getPathForScriptFileName:scriptFileName];
}

-(const char *)getPathForScriptFileName:(void *)scriptFileName {
    return (const char *)[[NSBundle mainBundle] pathForResource:(NSString *)scriptFileName ofType:@"lua"];
}

-(void)start {
    
    Physics *physics = new Physics();
    Renderer *renderer = new Renderer();
    
    finished = false;
    paused = false;
    while (!finished) {
        if (!paused) {
            physics->updatePhysics();
        }
        renderer->draw();
    }
    
}

@end
