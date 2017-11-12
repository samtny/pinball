//
// Created by samtny on 11/12/17.
//

#ifndef SHARED_FLIPPER_H
#define SHARED_FLIPPER_H

#include "shapeGroup.h"

class Flipper
{
public:
    Flipper(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody);

private:
    LayoutItem *item;
};

#endif //SHARED_FLIPPER_H
