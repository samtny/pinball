//
//  Flipper.h
//  shared
//
//  Created by Sam Thompson on 11/14/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#ifndef SHARED_FLIPPER_H
#define SHARED_FLIPPER_H

#include "Types.h"

struct LayoutItem;
class Physics;

class Flipper
{
public:
    Flipper(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics);
    void Flip(void);
    void Unflip(void);

private:
    LayoutItem *item;

    double _flipDirection;
    double _flipOffset;
    
    double _flipImpulse;
    double _flipForce;
    double _unflipImpulse;
    double _unflipForce;
};

#endif //SHARED_FLIPPER_H
