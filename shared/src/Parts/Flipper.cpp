//
// Created by samtny on 11/12/17.
//

#include "Parts/Flipper.h"
#include "Types.h"

static Physics *_physics_currentInstance;

void Flipper::Flip() {
    LayoutItem *flipper = this->item;
    
    cpBodyResetForces(flipper->bodies[0]);
    
    cpVect anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(1, 0), this->_flipOffset));
    
    cpBodyApplyImpulse(flipper->bodies[0], cpv(0, -this->_flipImpulse * this->_flipDirection), anchor);
    cpBodyApplyForce(flipper->bodies[0], cpv(0, -this->_flipForce * this->_flipDirection), anchor);
    
    anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(-1, 0), this->_flipOffset));
    
    cpBodyApplyImpulse(flipper->bodies[0], cpv(0, this->_flipImpulse * this->_flipDirection), anchor);
    cpBodyApplyForce(flipper->bodies[0], cpv(0, this->_flipForce * this->_flipDirection), anchor);
}

void Flipper::Unflip() {
    LayoutItem *flipper = this->item;

    cpBodyResetForces(flipper->bodies[0]);
    
    cpVect anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(1, 0), this->_flipOffset));
    
    cpBodyApplyImpulse(flipper->bodies[0], cpv(0, this->_unflipImpulse * this->_flipDirection), anchor);
    cpBodyApplyForce(flipper->bodies[0], cpv(0, this->_unflipForce * this->_flipDirection), anchor);
    
    anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(-1, 0), this->_flipOffset));
    
    cpBodyApplyImpulse(flipper->bodies[0], cpv(0, -this->_unflipImpulse * this->_flipDirection), anchor);
    cpBodyApplyForce(flipper->bodies[0], cpv(0, -this->_unflipForce * this->_flipDirection), anchor);
}

Flipper::Flipper(LayoutItem *item, shapeGroup shapeGroup, cpBody *attachBody, Physics *physics)
{
    this->item = item;
    
    _physics_currentInstance = physics;
    
    cpFloat area = (item->o->r1 * M_PI) * 2; // approx
    cpFloat mass = area * item->o->m->d;

    this->_flipDirection = (float)(item->v[0].x < item->v[1].x ? -1 : 1);
    this->_flipOffset = (float)cpvlength(cpvsub(item->v[0], item->v[1]));
    
    this->_flipImpulse = ::atof(item->o->meta.find("impulse")->second.c_str());
    this->_flipForce = ::atof(item->o->meta.find("force")->second.c_str());
    this->_unflipImpulse = ::atof(item->o->meta.find("unflipImpulse")->second.c_str());
    this->_unflipForce = ::atof(item->o->meta.find("unflipForce")->second.c_str());
    
    cpFloat length = cpvdist(item->v[0], item->v[1]);
    cpFloat flipAngle = this->_flipDirection * cpfacos(cpvdot(cpvnormalize(cpvsub(item->v[1],item->v[0])), cpvnormalize(cpvsub(item->v[2],item->v[0]))));

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

