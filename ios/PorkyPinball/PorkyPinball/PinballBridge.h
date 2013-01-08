//
//  PinballNative.h
//  PorkyPinball
//
//  Created by Richard Thompson on 11/8/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PinballBridgeInterface.h"

@interface PinballBridge : NSObject

-(void)initI;
-(void)setGameName:(const char *)gameName;
-(const char *)getGameName;
-(const char *)getPathForScriptFileName:(void *)scriptFileName;
-(const char *)getPathForTextureFileName:(void *)textureFileName;
-(GLTexture *)createRGBATexture:(void *)textureFileName;
-(HostProperties *)getHostProperties;
-(void)playSound:(const char *)soundName;
-(void)addTimer:(int)timerId duration:(float)duration;
-(void)setTimerDelegate:(ITimerDelegate *)timerDelegate;

@end
