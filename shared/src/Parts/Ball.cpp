//
//  Ball.cpp
//  shared
//
//  Created by Sam Thompson on 11/16/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#include <map>
#include <string>

#include "Parts/Ball.h"
#include "Types.h"

#include "Physics.h"
#include "PhysicsDelegate.h"

#include "Playfield.h"

static Physics *_physics_currentInstance;

static int ballCollisionGroup = 2048;

static Material _steelMaterial;

static void ballGravityVelocityFunc(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
    cpVect g = cpvrotate(cpvforangle(_physics_currentInstance->getBoxBody()->a), gravity);

    cpBodyUpdateVelocity(body, g, damping, dt);
}

static int __ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

    arb->e = _steelMaterial.e;
    arb->u = _steelMaterial.f;

    return 1;
}

Ball::Ball(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics) {
    this->item = item;
    cpSpace *_space = attachBody->space_private;
    _physics_currentInstance = physics;

    cpFloat area = (item->o->r1 * item->s * item->o->r1 * item->s * M_PI);
    cpFloat mass = area * item->o->m->d;
    
    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o->r1 * item->s, cpvzero)));
    cpBodySetPos(body, item->v[0]);
    body->velocity_func = ballGravityVelocityFunc;
    
    cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o->r1 * item->s, cpvzero));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    cpShapeSetGroup(shape, shapeGroupBall + ballCollisionGroup); // TODO: ball shape group is kludged
    cpShapeSetCollisionType(shape, CollisionTypeBall);
    cpShapeSetUserData(shape, item);
    
    body->data = item;
    
    ballCollisionGroup++;
    
    item->bodies.push_back(body);
    
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeBall, NULL, __ballPreSolve, NULL, NULL, NULL);
    
    std::map<std::string, Material> mats = *_physics_currentInstance->getPlayfield()->getMaterials();
    
    _steelMaterial = mats["steel"];
}
