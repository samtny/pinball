//
//  Ball.h
//  shared
//
//  Created by Sam Thompson on 11/16/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef Ball_h
#define Ball_h

#include "Types.h"

struct LayoutItem;
class Physics;

class Ball
{
public:
    Ball(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics);

private:
    LayoutItem *item;
};

#endif /* Ball_h */
