//
//  Switch.h
//  shared
//
//  Created by Sam Thompson on 11/16/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef Switch_h
#define Switch_h

#include "Types.h"

struct LayoutItem;
class Physics;

class Switch
{
public:
    Switch(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics);
    
private:
    LayoutItem *item;
};

#endif /* Switch_h */
