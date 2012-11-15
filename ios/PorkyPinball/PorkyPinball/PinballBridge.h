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
-(const char *)getPathForScriptFileName:(void *)scriptFileName;
-(const char *)getPathForTextureFileName:(void *)textureFileName;
-(Texture *)createRGBATexture:(void *)textureFileName;
-(DisplayProperties *)getDisplayProperties;
-(void)playSound:(void *)soundName;

@end
