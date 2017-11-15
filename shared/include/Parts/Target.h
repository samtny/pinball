//
//  PopBumper.h
//  shared
//
//  Created by Sam Thompson on 11/15/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef Target_h
#define Target_h

#include "Types.h"

struct LayoutItem;
class Physics;

class Target
{
public:
    Target(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics);
    
private:
    LayoutItem *item;
};

#endif /* Target_h */
