//
//  AppDelegate.h
//  PorkyPinball
//
//  Created by Richard Thompson on 11/7/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ViewController;
@class PinballHost;

@interface AppDelegate : UIResponder <UIApplicationDelegate> {
    PinballHost *pinballHost;
}

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) ViewController *viewController;

@property (nonatomic, retain) PinballHost *pinballHost;

@end
