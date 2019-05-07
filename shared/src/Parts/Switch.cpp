//
//  Switch.cpp
//  shared
//
//  Created by Sam Thompson on 11/16/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#include "Parts/Switch.h"
#include "Types.h"

#include "Physics.h"
#include "PhysicsDelegate.h"

static Physics *_physics_currentInstance;

static cpBool switchBegin(cpArbiter *arb, cpSpace *space, void *unused) {
    
    cpShape *a;
    cpShape *b;
    cpArbiterGetShapes(arb, &a, &b);
    
    LayoutItem *sw = (LayoutItem *)a->userData;
    LayoutItem *ball = (LayoutItem *)b->userData;
    
    if (sw && ball) {
        _physics_currentInstance->getDelegate()->switchClosed(sw->n.c_str(), ball->n.c_str());
    }
    
    return true;
    
}

static void switchSeparate(cpArbiter *arb, cpSpace *space, void *unused) {
    
    cpShape *a;
    cpShape *b;
    cpArbiterGetShapes(arb, &a, &b);
    
    LayoutItem *sw = (LayoutItem *)a->userData;
    LayoutItem *ball = (LayoutItem *)b->userData;
    
    if (sw && ball) {
        _physics_currentInstance->getDelegate()->switchOpened(sw->n.c_str(), ball->n.c_str());
    }
    
}

Switch::Switch(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics) {
    this->item = item;
    cpSpace *_space = attachBody->space;
    _physics_currentInstance = physics;
    
    cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(attachBody, cpBodyWorldToLocal(attachBody, item->v[0]), cpBodyWorldToLocal(attachBody, item->v[1]), item->o->r1));
    cpShapeSetSensor(shape, true);
    cpShapeSetCollisionType(shape, CollisionTypeSwitch);
    cpShapeSetUserData(shape, item);
    
    item->shapes.push_back(shape);
    
    //cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeSwitch, switchBegin, NULL, NULL, switchSeparate, NULL);
    cpCollisionHandler *handler = cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeSwitch);
    handler->beginFunc = switchBegin;
    handler->separateFunc = switchSeparate;
}
