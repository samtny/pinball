//
//  PinballNative.h
//  PorkyPinball
//
//  Created by Richard Thompson on 11/8/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import "PinballNativeInterface.h"

@interface PinballNative : NSObject

-(const char *)getPathForScriptFileName:(void *)scriptFileName;
-(DisplayProperties *)getDisplayProperties;
-(void)playSound:(void *)soundName;

@end
