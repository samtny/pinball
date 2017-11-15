//
//  Slingshot.h
//  shared
//
//  Created by Sam Thompson on 11/14/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef Slingshot_h
#define Slingshot_h

#include "Types.h"

struct LayoutItem;
class Physics;

class Slingshot
{
public:
    Slingshot(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics);
    
private:
    LayoutItem *item;
};

#endif /* Slingshot_h */
