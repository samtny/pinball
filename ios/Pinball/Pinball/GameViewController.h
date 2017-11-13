//
//  ViewController.h
//  PorkyPinball
//
//  Created by Richard Thompson on 11/7/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <OpenGLES/ES2/glext.h>

#import "PinballGame.h"

@interface GameViewController : GLKViewController

@property (strong, nonatomic) PinballGame *game;

-(IBAction)userDidTapStartButton:(id)sender;
-(IBAction)userDidTapLeftFlipperButton:(id)sender;
-(IBAction)userDidReleaseLeftFlipperButton:(id)sender;
-(IBAction)userDidTapRightFlipperButton:(id)sender;
-(IBAction)userDidReleaseRightFlipperButton:(id)sender;

@end
