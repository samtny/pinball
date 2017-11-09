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
-(const char *)getScriptPath:(const char *)scriptFileName;
-(const char *)getTexturePath:(const char *)textureFileName;
-(GLTexture *)createRGBATexture:(void *)textureFileName;
-(const HostProperties *)getHostProperties;
-(void)playSound:(const char *)soundName;
-(void)addTimer:(int)timerId duration:(float)duration delegate:(const ITimerDelegate *)timerDelegate;

@end
