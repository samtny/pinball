//
//  Slingshot.h
//  shared
//
//  Created by Sam Thompson on 11/14/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef Slingshot_h
#define Slingshot_h

#include "shapeGroup.h"

class Slingshot
{
public:
    Slingshot(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody);
    
private:
    LayoutItem *item;
};

#endif /* Slingshot_h */
