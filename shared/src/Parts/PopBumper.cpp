//
//  PopBumper.cpp
//  shared
//
//  Created by Sam Thompson on 11/15/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#include "Parts/PopBumper.h"
#include "Types.h"

#include "Physics.h"
#include "PhysicsDelegate.h"

static double _popBumperThreshold = 0;
static double _popBumperImpulse = 0;

static Physics *_physics_currentInstance;

static void popBumperPostSolve(cpArbiter *arb, cpSpace *space, void *unused) {
    
    if (!(arb->CP_PRIVATE(state) == cpArbiterStateFirstColl)) return;
    
    cpVect impulse = cpArbiterTotalImpulse(arb);
    
    if (cpvlength(impulse) >= _popBumperThreshold) {
        
        cpBody *ball, *pop;
        cpArbiterGetBodies(arb, &ball, &pop);
        
        cpVect normal = cpArbiterGetNormal(arb, 0);
        
        cpBodyApplyImpulse(ball, cpvmult(normal, -_popBumperImpulse), cpvzero);
        cpBodyApplyImpulse(pop, cpvmult(normal, _popBumperImpulse), cpvzero);
        
        LayoutItem *l = (LayoutItem *)pop->data;
        
        _physics_currentInstance->getDelegate()->switchClosed(l->n.c_str(), nullptr);
        
    }
}

PopBumper::PopBumper(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics) {
    this->item = item;
    _physics_currentInstance = physics;
    
    cpFloat area = (item->o->r1 * item->o->r1 * M_PI);
    cpFloat mass = area * item->o->m->d;
    
    _popBumperImpulse = ::atof(item->o->meta.find("impulse")->second.c_str());
    _popBumperThreshold = ::atof(item->o->meta.find("threshold")->second.c_str());
    
    cpSpace *_space = attachBody->space_private;
    
    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o->r1, cpvzero)));
    cpBodySetPos(body, cpv(item->v[0].x, item->v[0].y));
    
    cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o->r1, cpvzero));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    cpShapeSetCollisionType(shape, CollisionTypePopbumper);
    cpShapeSetGroup(shape, shapeGroupPopbumpers);
    cpShapeSetUserData(shape, item);
    
    cpConstraint *constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(attachBody, body, body->p));
    constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(attachBody, body, 0.0f, 0.0f));
    
    body->data = item;
    
    item->bodies.push_back(body);
    
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypePopbumper, NULL, NULL, popBumperPostSolve, NULL, NULL);
}
