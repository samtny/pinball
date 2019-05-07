//
// Created by samtny on 11/12/17.
//

#include "Parts/Slingshot.h"
#include "Types.h"

static double _slingshotImpulse = 0;

static Physics *_physics_currentInstance;

static cpBool slingshotSwitchBegin(cpArbiter *arb, cpSpace *space, void *userData) {

    cpBody *slingshot, *sw;
    cpArbiterGetBodies(arb, &slingshot, &sw);
    
    cpVect normal = cpArbiterGetNormal(arb);
    
    cpBodyApplyImpulseAtWorldPoint(slingshot, cpvmult(normal, -_slingshotImpulse), cpvzero);
    
    /*
     LayoutItem *l = (LayoutItem *)slingshot->data;
     
     physics_currentInstance->getDelegate()->switchClosed(l->n.c_str());
     */
    return 1;
    
}

Slingshot::Slingshot(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics) {
    this->item = item;
    _physics_currentInstance = physics;

    cpVect mid = cpvmult(cpvadd(item->v[0], item->v[1]), 0.5);
    
    cpFloat area = (item->o->r1 * M_PI) * 2; // approx
    cpFloat mass = area * item->o->m->d;
    
    cpSpace *_space = attachBody->space;
    
    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForSegment(mass, item->v[0], item->v[1], item->o->r1)));
    cpBodySetPosition(body, mid);
    
    // target surface normal;
    cpVect normal = cpvnormalize(cpvperp(cpvsub(item->v[0], item->v[1])));
    
    cpFloat _slingshotRestLength = ::atof(item->o->meta.find("restLength")->second.c_str());
    cpFloat _slingshotStiffness = ::atof(item->o->meta.find("stiffness")->second.c_str());
    cpFloat _slingshotDamping = ::atof(item->o->meta.find("damping")->second.c_str());
    cpFloat _slingshotSwitchGap = ::atof(item->o->meta.find("switchGap")->second.c_str());
    _slingshotImpulse = ::atof(item->o->meta.find("impulse")->second.c_str());
    
    // groove
    cpVect grooveA = cpvadd(body->p, cpvmult(normal, _slingshotRestLength));
    cpVect grooveB = body->p;
    
    cpConstraint *constraint = cpSpaceAddConstraint(_space, cpGrooveJointNew(attachBody, body, cpBodyWorldToLocal(attachBody, grooveA), cpBodyWorldToLocal(attachBody, grooveB), cpvzero));
    constraint = cpSpaceAddConstraint(_space, cpDampedSpringNew(attachBody, body, cpBodyWorldToLocal(attachBody, grooveA), cpvzero, _slingshotRestLength, _slingshotStiffness, _slingshotDamping));
    constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, attachBody, 0.0f, 0.0f));
    
    cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(item->v[0], body->p), cpvsub(item->v[1], body->p), item->o->r1));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    
    cpShapeFilter filter = cpShapeFilterNew(shapeGroupSlingshots, ~CP_ALL_CATEGORIES, ~CP_ALL_CATEGORIES);
    cpShapeSetFilter(shape, filter);

    cpShapeSetCollisionType(shape, CollisionTypeSlingshot);
    cpShapeSetUserData(shape, item);
    
    // switch
    shape = cpSpaceAddShape(_space, cpCircleShapeNew(attachBody, _slingshotRestLength - _slingshotSwitchGap, cpBodyWorldToLocal(attachBody, grooveA)));
    cpShapeSetSensor(shape, true);
    cpShapeSetCollisionType(shape, CollisionTypeSlingshotSwitch);
    
    item->shapes.push_back(shape);
    
    item->bodies.push_back(body);
    
    cpCollisionHandler *collisionHandler = cpSpaceAddCollisionHandler(_space, CollisionTypeData[CollisionTypeSlingshot], CollisionTypeData[CollisionTypeSlingshotSwitch]);
    
    collisionHandler->beginFunc = slingshotSwitchBegin;
}
