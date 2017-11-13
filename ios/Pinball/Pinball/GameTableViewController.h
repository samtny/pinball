//
//  GameTableViewController.h
//  Pinball
//
//  Created by Sam Thompson on 11/13/17.
//  Copyright © 2017 It-Just-Works Software, llc. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "PinballGame.h"

@interface GameTableViewController : UITableViewController

@property (strong, nonatomic) PinballGame *game;

@end
