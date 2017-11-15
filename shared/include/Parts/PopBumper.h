//
//  PopBumper.h
//  shared
//
//  Created by Sam Thompson on 11/15/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef PopBumper_h
#define PopBumper_h

#include "Types.h"

struct LayoutItem;
class Physics;

class PopBumper
{
public:
    PopBumper(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics);
    
private:
    LayoutItem *item;
};

#endif /* PopBumper_h */
