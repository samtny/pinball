//
//  PopBumper.cpp
//  shared
//
//  Created by Sam Thompson on 11/15/17.
//  Copyright © 2017 Sam Thompson. All rights reserved.
//

#include "Parts/Target.h"
#include "Types.h"

#include "Physics.h"
#include "PhysicsDelegate.h"

static Physics *_physics_currentInstance;

static cpBool targetSwitchBegin(cpArbiter *arb, cpSpace *space, void *unused) {
    
    cpShape *target, *sw;
    cpArbiterGetShapes(arb, &target, &sw);
    
    LayoutItem *l = (LayoutItem *)target->userData;
    
    if (l) {
        _physics_currentInstance->getDelegate()->switchClosed(l->n.c_str(), NULL);
    }
    
    // we don't need more information from this
    return 0;
    
}

Target::Target(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics) {
    this->item = item;
    cpSpace *_space = attachBody->space;
    _physics_currentInstance = physics;
    
    double _targetRestLength = ::atof(item->o->meta.find("restLength")->second.c_str());
    double _targetStiffness = ::atof(item->o->meta.find("stiffness")->second.c_str());
    double _targetSwitchGap = ::atof(item->o->meta.find("switchGap")->second.c_str());
    double _targetDamping = ::atof(item->o->meta.find("damping")->second.c_str());
    
    cpVect a = cpv(item->v[0].x, item->v[0].y);
    cpVect b = cpv(item->v[1].x, item->v[1].y);
    cpVect mid = cpvmult(cpvadd(a, b), 0.5);
    
    cpFloat length = cpvdist(a, b);
    
    cpFloat area = (item->o->r1 * length) * 2;
    cpFloat mass = area * item->o->m->d;

    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForSegment(mass, a, b, item->o->r1)));
    cpBodySetPosition(body, mid);
    
    // target surface normal;
    cpVect targetNormal = cpvnormalize(cpvperp(cpvsub(a, b)));
    
    // groove
    cpVect grooveA = cpvadd(body->p, cpvmult(targetNormal, _targetRestLength));
    cpVect grooveB = body->p;
    
    cpConstraint *constraint = cpSpaceAddConstraint(_space, cpGrooveJointNew(attachBody, body, cpBodyWorldToLocal(attachBody, grooveA), cpBodyWorldToLocal(attachBody, grooveB), cpvzero));
    constraint = cpSpaceAddConstraint(_space, cpDampedSpringNew(attachBody, body, cpBodyWorldToLocal(attachBody, grooveA), cpvzero, _targetRestLength, _targetStiffness, _targetDamping));
    constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, attachBody, 0.0f, 0.0f));
    
    cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(a, body->p), cpvsub(b, body->p), item->o->r1));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    cpShapeSetCollisionType(shape, CollisionTypeTarget);
    
    cpShapeFilter filter = cpShapeFilterNew(shapeGroupTargets, ~CP_ALL_CATEGORIES, ~CP_ALL_CATEGORIES);
    cpShapeSetFilter(shape, filter);

    cpShapeSetUserData(shape, item);
    
    // switch
    shape = cpSpaceAddShape(_space, cpCircleShapeNew(attachBody, _targetRestLength - _targetSwitchGap, cpBodyWorldToLocal(attachBody, grooveA)));
    cpShapeSetSensor(shape, true);
    cpShapeSetCollisionType(shape, CollisionTypeTargetSwitch);
    
    item->shapes.push_back(shape);
    
    item->bodies.push_back(body);
    
    cpCollisionHandler *handler = cpSpaceAddCollisionHandler(_space, CollisionTypeData[CollisionTypeSlingshot], CollisionTypeData[CollisionTypeSwitch]);
    handler->beginFunc = targetSwitchBegin;
}
