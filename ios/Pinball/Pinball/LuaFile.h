//
//  LuaFile.h
//  Pinball
//
//  Created by Sam Thompson on 11/13/17.
//  Copyright © 2017 It-Just-Works Software, llc. All rights reserved.
//

#ifndef LuaFile_h
#define LuaFile_h

#import <Foundation/Foundation.h>

@interface LuaFile : NSObject

@property (strong, nonatomic) NSString *originalFilePath;
@property (strong, nonatomic) NSString *saveFilePath;

@end

#endif /* LuaFile_h */
