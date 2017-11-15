//
// Created by samtny on 11/12/17.
//

#include "Parts/Flipper.h"
#include "Types.h"

static Physics *_physics_currentInstance;

Flipper::Flipper(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics)
{
    this->item = item;
    
    _physics_currentInstance = physics;

    cpFloat area = (item->o->r1 * M_PI) * 2; // approx
    cpFloat mass = area * item->o->m->d;

    cpFloat direction = item->v[0].x <= item->v[1].x ? -1 : 1; // rotate clockwise for right-facing flipper...

    cpFloat length = cpvdist(item->v[0], item->v[1]);
    cpFloat flipAngle = direction * cpfacos(cpvdot(cpvnormalize(cpvsub(item->v[1],item->v[0])), cpvnormalize(cpvsub(item->v[2],item->v[0]))));

    cpFloat flipStart = flipAngle > 0 ? 0 : flipAngle;
    cpFloat flipEnd = flipAngle > 0 ? flipAngle : 0;

    cpSpace *_space = attachBody->space_private;

    // flipper body is round centered at base of flipper, and for this implementation has radius == flipper length;
    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, length, cpvzero)));
    cpBodySetPos(body, item->v[0]);

    cpConstraint *constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, attachBody, item->v[0]));
    constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, attachBody, flipStart, flipEnd));

    // base shape
    cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o->r1, cpvzero));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    cpShapeSetGroup(shape, shapeGroup);

    float diff = item->o->r1 - item->o->r2; // r1-prime
    float loft = atan2f(diff, (float)length);
    float facelen = diff / sin(loft);
    cpVect p2p1 = cpvsub(item->v[0], item->v[1]);
    cpVect p2p1n = cpvnormalize(p2p1);
    cpVect p3n = cpvrotate(p2p1n, cpvforangle(loft));
    cpVect p3 = cpvadd(item->v[1], cpvmult(p3n, facelen));

    // face shapes
    shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(item->v[1], body->p), cpvsub(p3, body->p), item->o->r2));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    cpShapeSetGroup(shape, shapeGroup);

    loft = -atan2f(diff, (float)length);
    p3n = cpvrotate(p2p1n, cpvforangle(loft));
    p3 = cpvadd(item->v[1], cpvmult(p3n, facelen));

    shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(item->v[1], body->p), cpvsub(p3, body->p), item->o->r2));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
    cpShapeSetGroup(shape, shapeGroup);

    cpBodySetUserData(body, item);

    item->bodies.push_back(body);
}

