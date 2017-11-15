//
//  PopBumper.h
//  shared
//
//  Created by Sam Thompson on 11/15/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef Kraken_h
#define Kraken_h

#include "Types.h"

struct LayoutItem;
class Physics;

class Kraken
{
public:
    Kraken(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics);
    
private:
    LayoutItem *item;
};

#endif /* Kraken_h */
