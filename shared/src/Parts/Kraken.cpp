//
//  PopBumper.cpp
//  shared
//
//  Created by Sam Thompson on 11/15/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#include "Parts/Kraken.h"
#include "Types.h"

#include "Physics.h"
#include "PhysicsDelegate.h"

static Physics *_physics_currentInstance;

static cpBodyVelocityFunc ballGravityVelocityFuncPrevious;

static void ballGravityVelocityFuncKraken(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
    cpVect gBox = cpvrotate(cpvforangle(_physics_currentInstance->getBoxBody()->a), gravity);
    
    cpFloat krakenGravity = -2.0;
    
    // Gravitational acceleration is proportional to the inverse square of
    // distance, and directed toward the origin. The central planet is assumed
    // to be massive enough that it affects the satellites but not vice versa.
    
    LayoutItem *kraken = _physics_currentInstance->getLayoutItem("kraken");
    
    //LayoutItem *kraken = _playfield->getLayout()->find("kraken")->second;
    
    cpBody *krakenBody = kraken->bodies[0];
    
    cpVect p = krakenBody->p;
    
    cpFloat sqdist = cpvlengthsq(p);
    
    cpVect gKraken = cpvmult(p, -krakenGravity / (sqdist * cpfsqrt(sqdist)));
    
    //cpVect g = cpvadd(gBox, gKraken);
    cpVect g = gKraken;
    
    cpBodyUpdateVelocity(body, g, damping, dt);
}

static int krakenBegin(cpArbiter *arb, cpSpace *space, void *unused) {
    
    printf("%s", "begin");
    
    cpBody *ball, *kraken;
    cpArbiterGetBodies(arb, &ball, &kraken);

    ballGravityVelocityFuncPrevious = ball->velocity_func;
    
    ball->velocity_func = ballGravityVelocityFuncKraken;
    
    return 1;
}

static void krakenSeparate(cpArbiter *arb, cpSpace *space, void *unused) {
    printf("%s", "separate");
    
    cpBody *ball, *kraken;
    cpArbiterGetBodies(arb, &ball, &kraken);
    
    ball->velocity_func = ballGravityVelocityFuncPrevious;
}

Kraken::Kraken(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics) {
    this->item = item;
    cpSpace *_space = attachBody->space_private;
    _physics_currentInstance = physics;
    
    cpVect a = cpv(item->v[0].x, item->v[0].y);
    
    cpFloat area = (item->o->r1 * item->o->r1) * M_PI;
    cpFloat mass = area * item->o->m->d;
    
    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o->r1, cpvzero)));
    cpBodySetPos(body, a);
    
    cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o->r1, cpvzero));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    
    cpShapeSetSensor(shape, true);
    cpShapeSetCollisionType(shape, CollisionTypeKraken);
    
    cpShapeSetGroup(shape, shapeGroupKraken);
    cpShapeSetUserData(shape, item);
    
    cpConstraint *constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(attachBody, body, body->p));
    
    //constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(box->bodies[0], body, 0.0f, 0.0f));
    
    //cpBodyApplyForce(body, cpv(0.0, -1.0), cpv(item->o->r1, 0.0));
    
    cpSpaceAddConstraint(_space, cpSimpleMotorNew(attachBody, body, 2.0));
    
    body->data = item;
    
    item->bodies.push_back(body);
    
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeKraken, krakenBegin, NULL, NULL, krakenSeparate, NULL);
}
