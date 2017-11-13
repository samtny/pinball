
#include "Physics.h"

#include "PinballBridgeInterface.h"

#include "PhysicsDelegate.h"

#include "Playfield.h"

#include "Parts/Flipper.h"

#include "Util.h"

#include "chipmunk/chipmunk.h"

#include <iostream>

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

using std::string;
using std::map;

static Physics *physics_currentInstance;

//static cpSpace *_space;

static cpVect gravity = cpv(0.0, 9.80665f);

static double _boxStiffness = 200.0;
static double _boxDamping = 100.0;

static double flipImpulse = 0.02f;
static double unflipImpulse = 0.02f;

static double flipForce = 0.2f;
static double unflipForce = 0.2f;

static double _targetRestLength = 0;
static double _targetSwitchGap = 0;
static double _targetStiffness = 0;
static double _targetDamping = 0;

static double _slingshotRestLength = 0.6;
static double _slingshotSwitchGap = 0.25;
static double _slingshotStiffness = 35.0;
static double _slingshotDamping = 0.05;
static double _slingshotImpulse = 0.02;

static double _popBumperThreshold = 0.005;
static double _popBumperImpulse = 0.04;

static double _nudgeImpulse = 1.0;

static double timeStep = 1.0/180.0;

static int iterations = 10;

static float scale = 37;

enum CollisionType {
	CollisionTypeNone,
	CollisionTypeSwitch,
	CollisionTypeBall,
	CollisionTypeTarget,
	CollisionTypeTargetSwitch,
	CollisionTypePopbumper,
	CollisionTypeSlingshot,
	CollisionTypeSlingshotSwitch,
    CollisionTypeKraken
};

Physics::Physics(void)
{
	physics_currentInstance = this;
	_paused = false;
}

void Physics::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Physics::setDelegate(IPhysicsDelegate *delegate) {
	_delegate = delegate;
}

IPhysicsDelegate * Physics::getDelegate() {
	return _delegate;
}

void Physics::setPlayfield(Playfield *playfield) {
	_playfield = playfield;
}

void Physics::init() {

	this->loadConfig();
    this->loadForces();

	_space = cpSpaceNew();
	cpSpaceSetIterations(_space, iterations);
	//cpSpaceSetCollisionSlop(_space, 0.9);
	//cpSpaceSetDamping(_space, 0.1);
	
	// TODO: move to properties of item/object;
	_targetRestLength *= 1 / scale;
	_targetSwitchGap *= 1 / scale;
	_slingshotRestLength *= 1 / scale;
	_slingshotSwitchGap *= 1 / scale;

	this->createObjects();
	
	this->initCollisionHandlers();

	cpSpaceSetGravity(_space, gravity);

}

static int __ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	return physics_currentInstance->ballPreSolve(arb, space, unused);

}

int Physics::ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	// TODO: move to local var;
	map<string, Material> mats = *_playfield->getMaterials();

	Material mat = mats["steel"];

	arb->e = mat.e;
	arb->u = mat.f;

	return 1;

}

static int switchBegin(cpArbiter *arb, cpSpace *space, void *unused) {

	cpShape *a;
	cpShape *b;
	cpArbiterGetShapes(arb, &a, &b);

	LayoutItem *sw = (LayoutItem *)a->data;
	LayoutItem *ball = (LayoutItem *)b->data;
	
    if (sw && ball) {
        physics_currentInstance->getDelegate()->switchClosed(sw->n.c_str(), ball->n.c_str());
    }

	return 1;

}

static void switchSeparate(cpArbiter *arb, cpSpace *space, void *unused) {

	cpShape *a;
	cpShape *b;
	cpArbiterGetShapes(arb, &a, &b);

	LayoutItem *sw = (LayoutItem *)a->data;
	LayoutItem *ball = (LayoutItem *)b->data;

    if (sw && ball) {
        physics_currentInstance->getDelegate()->switchOpened(sw->n.c_str(), ball->n.c_str());
    }

}

static void popBumperPostSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	if (!(arb->CP_PRIVATE(state) == cpArbiterStateFirstColl)) return;

	cpVect impulse = cpArbiterTotalImpulse(arb);
	
	if (cpvlength(impulse) >= _popBumperThreshold) {
		
		cpBody *ball, *pop;
		cpArbiterGetBodies(arb, &ball, &pop);

		cpVect normal = cpArbiterGetNormal(arb, 0);

		cpBodyApplyImpulse(ball, cpvmult(normal, -_popBumperImpulse), cpvzero);
		
		LayoutItem *l = (LayoutItem *)pop->data;

		physics_currentInstance->getDelegate()->switchClosed(l->n.c_str(), NULL);

	}

}

static int targetSwitchBegin(cpArbiter *arb, cpSpace *space, void *unused) {

	cpShape *target, *sw;
	cpArbiterGetShapes(arb, &target, &sw);

	LayoutItem *l = (LayoutItem *)target->data;

    if (l) {
        physics_currentInstance->getDelegate()->switchClosed(l->n.c_str(), NULL);
    }

	// we don't need more information from this
	return 0;

}

static int slingshotSwitchBegin(cpArbiter *arb, cpSpace *space, void *unused) {

	cpBody *slingshot, *sw;
	cpArbiterGetBodies(arb, &slingshot, &sw);

	cpVect normal = cpArbiterGetNormal(arb, 0);

	cpBodyApplyImpulse(slingshot, cpvmult(normal, -_slingshotImpulse), cpvzero);
		
	/*
	LayoutItem *l = (LayoutItem *)slingshot->data;

	physics_currentInstance->getDelegate()->switchClosed(l->n.c_str());
	*/
	return 1;

}

cpSpace *Physics::getSpace() {
	return _space;
}
/*
void Physics::applyScale(LayoutItem *iprops) {

	for (int i = 0; i < iprops->count; i++) {
		iprops->v[i].x *= 1 / scale;
		iprops->v[i].y *= 1 / scale;
	}

	float localScale = scale * 1 / iprops->s;

	iprops->o.r1 *= 1 / localScale;
	iprops->o.r2 *= 1 / localScale;
	
	//iprops->o.t.s *= 1 / scale;
	
}
*/
float Physics::getScale() {
	return scale;
}

static void destroyConstraint(cpBody *body, cpConstraint *constraint, void *data) {
	cpSpace *space = cpBodyGetSpace(body);
	cpSpaceRemoveConstraint(space, constraint);
	cpConstraintFree(constraint);
}

static void destroyShape(cpBody *body, cpShape *shape, void *data) {
	cpSpace *space = cpBodyGetSpace(body);
	cpSpaceRemoveShape(space, shape);
	cpShapeFree(shape);
}

static void destroyObject(cpSpace *space, void *itm, void *unused) {
	// TODO: yep
	/*
	LayoutItem *item = (LayoutItem *)itm;

	LayoutItem box = physics_currentInstance->getLayoutItems()->find("box")->second;

	if (item->body && (strcmp(item->n.c_str(), "box") == 0 || item->body != box.body)) {
		cpBodyEachConstraint(item->body, destroyConstraint, NULL);
		cpBodyEachShape(item->body, destroyShape, NULL);
		cpSpaceRemoveBody(space, item->body);
		cpBodyFree(item->body);
	} else if (item->shape) {
		cpSpaceRemoveShape(space, item->shape);
		cpShapeFree(item->shape);
	}
	*/
	LayoutItem *item = (LayoutItem *)itm;

	for (int i = 0; i < (int)item->bodies.size(); i++) {
		cpBody *body = item->bodies[i];
		cpBodyEachConstraint(body, destroyConstraint, NULL);
		cpBodyEachShape(body, destroyShape, NULL);
		cpSpaceRemoveBody(space, body);
		cpBodyFree(body);
	}
	item->bodies.clear();

	for (int i = 0; i < (int)item->shapes.size(); i++) {
		cpShape *shape = item->shapes[i];
		cpSpaceRemoveShape(space, shape);
		cpShapeFree(shape);
	}
	item->shapes.clear();

}

void Physics::destroyObject(LayoutItem *item) {

	if (cpSpaceIsLocked(_space)) {
		cpSpaceAddPostStepCallback(_space, ::destroyObject, item, NULL);
	} else {
		::destroyObject(_space, item, NULL);
	}

}

void Physics::createObjects(void) {

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;
	this->createObject(box);

	for (it_LayoutItem iterator = _playfield->getLayout()->begin(); iterator != _playfield->getLayout()->end(); iterator++) {
		LayoutItem *lprops = &(&*iterator)->second;
		if (strcmp(lprops->n.c_str(), "box") != 0) {
			this->createObject(lprops);
		}
	}

}

void Physics::createObject(LayoutItem *item) {
	
	if (strcmp(item->o->s.c_str(), "box") == 0) {
		this->createBox(item);
		// TODO: goeth elsewhere;
		item->width = (float)(item->v[3].x - item->v[0].x);
		item->height = (float)(item->v[1].y - item->v[0].y);
	} else if (strcmp(item->o->s.c_str(), "segment") == 0) {
		this->createSegment(item);
	} else if (strcmp(item->o->s.c_str(), "flipper") == 0) {
		new Flipper(item, shapeGroupFlippers, _boxBody);
	} else if (strcmp(item->o->s.c_str(), "ball") == 0) {
        this->createBall(item);
    } else if (strcmp(item->o->s.c_str(), "switch") == 0) {
		this->createSwitch(item);
	} else if (strcmp(item->o->s.c_str(), "circle") == 0) {
		this->createCircle(item);
	} else if (strcmp(item->o->s.c_str(), "target") == 0) {
		this->createTarget(item, NULL);
	} else if (strcmp(item->o->s.c_str(), "popbumper") == 0) {
		this->createPopbumper(item);
	} else if (strcmp(item->o->s.c_str(), "slingshot") == 0) {
		this->createSlingshot(item);
    } else if (strcmp(item->o->s.c_str(), "kraken") == 0) {
        this->createKraken(item);
    }

}

void Physics::createBox(LayoutItem *item) {

	cpBody *body, *staticBody = cpSpaceGetStaticBody(_space);
	cpShape *shape;
	cpConstraint *constraint;

	cpFloat area = (item->v[1].y - item->v[0].y) * (item->v[3].x - item->v[0].x);
	cpFloat mass = area * item->o->m->d;
	
	// create body on which to hang the "box";
	body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForPoly(mass, 4, (const cpVect *)&item->v[0], cpvzero)));
	
	// the implications of attaching all playfield objects to this non-zero-indexed body are not inconsequential...
	cpBodySetPos(body, cpvmult(cpvadd(item->v[2], item->v[0]), 0.5f));
	
	// pin the box body at the four corners;
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[0]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[1]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[2]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[3]));
	
	// pin the box in place;
	constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, cpvmult(cpvadd(cpvmult(cpvadd(item->v[0], item->v[1]), 0.5), item->v[2]), 0.5)) );
	//constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, staticBody, 0.0f, 0.0f));
	constraint = cpSpaceAddConstraint(_space, cpDampedRotarySpringNew(body, staticBody, 0.0f, _boxStiffness, _boxDamping) );
	
	// hang the box shapes on the body;
	// left
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[0]), cpBodyWorld2Local(body, item->v[1]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// top
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[1]), cpBodyWorld2Local(body, item->v[2]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// right
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[2]), cpBodyWorld2Local(body, item->v[3]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// bottom
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[3]), cpBodyWorld2Local(body, item->v[0]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	cpBodySetUserData(body, item);

	item->bodies.push_back(body);

	_boxBody = body;

}

cpBody *Physics::getBoxBody() {
	return _boxBody;
}

static int ballCollisionGroup = 2048;

static void
ballGravityVelocityFunc(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
	cpVect g = cpvrotate(cpvforangle(physics_currentInstance->getBoxBody()->a), gravity);
	
	cpBodyUpdateVelocity(body, g, damping, dt);
}

void Physics::createBall(LayoutItem *item) {
    
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

}

void Physics::createFlipper(LayoutItem *item) {

}

void Physics::createSwitch(LayoutItem *item) {

	// TODO: meh
	LayoutItem *box = &_playfield->getLayout()->find("box")->second;
	
	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(box->bodies[0], cpBodyWorld2Local(box->bodies[0], item->v[0]), cpBodyWorld2Local(box->bodies[0], item->v[1]), item->o->r1));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeSwitch);
	cpShapeSetUserData(shape, item);

	item->shapes.push_back(shape);
		
}

void Physics::createSegment(LayoutItem *item) {

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;

	for (int i = 0; i < item->count-1; i++) {

		cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(box->bodies[0], cpBodyWorld2Local(box->bodies[0], item->v[i]), cpBodyWorld2Local(box->bodies[0], item->v[i+1]), item->o->r1));
		cpShapeSetElasticity(shape, item->o->m->e);
		cpShapeSetFriction(shape, item->o->m->f);
		cpShapeSetUserData(shape, item);

		item->shapes.push_back(shape);

	}
	
}

void Physics::createSlingshot(LayoutItem *item) {

	cpVect mid = cpvmult(cpvadd(item->v[0], item->v[1]), 0.5);

	cpFloat area = (item->o->r1 * M_PI) * 2; // approx
	cpFloat mass = area * item->o->m->d;

	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForSegment(mass, item->v[0], item->v[1])));
	cpBodySetPos(body, mid);

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;

 	// target surface normal;
	cpVect normal = cpvnormalize(cpvperp(cpvsub(item->v[0], item->v[1])));

	// groove
	cpVect grooveA = cpvadd(body->p, cpvmult(normal, _slingshotRestLength));
	cpVect grooveB = body->p;

	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpGrooveJointNew(box->bodies[0], body, cpBodyWorld2Local(box->bodies[0], grooveA), cpBodyWorld2Local(box->bodies[0], grooveB), cpvzero));
	constraint = cpSpaceAddConstraint(_space, cpDampedSpringNew(box->bodies[0], body, cpBodyWorld2Local(box->bodies[0], grooveA), cpvzero, _slingshotRestLength, _slingshotStiffness, _slingshotDamping));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, box->bodies[0], 0.0f, 0.0f));

	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(item->v[0], body->p), cpvsub(item->v[1], body->p), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupSlingshots);
	cpShapeSetCollisionType(shape, CollisionTypeSlingshot);
	cpShapeSetUserData(shape, item);

	// switch
	shape = cpSpaceAddShape(_space, cpCircleShapeNew(box->bodies[0], _slingshotRestLength - _slingshotSwitchGap, cpBodyWorld2Local(box->bodies[0], grooveA)));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeSlingshotSwitch);

	item->shapes.push_back(shape);

	item->bodies.push_back(body);

}

void Physics::createCircle(LayoutItem *item) {

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;

	cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(box->bodies[0], item->o->r1, cpBodyWorld2Local(box->bodies[0], item->v[0])));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetUserData(shape, item);

	item->shapes.push_back(shape);
	
}

LayoutItem *Physics::getLayoutItem(const char *itemName) {
    return &_playfield->getLayout()->find(itemName)->second;
}

static void ballGravityVelocityFuncKraken(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
    cpVect gBox = cpvrotate(cpvforangle(physics_currentInstance->getBoxBody()->a), gravity);
    
    cpFloat krakenGravity = -2.0;
    
    // Gravitational acceleration is proportional to the inverse square of
    // distance, and directed toward the origin. The central planet is assumed
    // to be massive enough that it affects the satellites but not vice versa.
    
    LayoutItem *kraken = physics_currentInstance->getLayoutItem("kraken");
    
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
    
    ball->velocity_func = ballGravityVelocityFuncKraken;
    
    return 1;
}

static void krakenSeparate(cpArbiter *arb, cpSpace *space, void *unused) {
    printf("%s", "separate");
    
    cpBody *ball, *kraken;
    cpArbiterGetBodies(arb, &ball, &kraken);
    
    ball->velocity_func = ballGravityVelocityFunc;
}

void Physics::createKraken(LayoutItem *item) {
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
    
    LayoutItem *box = &_playfield->getLayout()->find("box")->second;
    
    cpConstraint *constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(box->bodies[0], body, body->p));
    
    //constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(box->bodies[0], body, 0.0f, 0.0f));
    
    //cpBodyApplyForce(body, cpv(0.0, -1.0), cpv(item->o->r1, 0.0));
    
    cpSpaceAddConstraint(_space, cpSimpleMotorNew(box->bodies[0], body, 2.0));
    
    body->data = item;
    
    item->bodies.push_back(body);
}

void Physics::createTarget(LayoutItem *item, void *attach) {

    LayoutItem *attachItem;
    
    if (!attach) {
        attachItem = &_playfield->getLayout()->find("box")->second;
    } else {
        attachItem = (LayoutItem *)attach;
    }
    
	cpVect a = cpv(item->v[0].x, item->v[0].y);
	cpVect b = cpv(item->v[1].x, item->v[1].y);
	cpVect mid = cpvmult(cpvadd(a, b), 0.5);

	cpFloat length = cpvdist(a, b);

	cpFloat area = (item->o->r1 * length) * 2;
	cpFloat mass = area * item->o->m->d;

	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForSegment(mass, a, b)));
	cpBodySetPos(body, mid);
	
	// target surface normal;
	cpVect targetNormal = cpvnormalize(cpvperp(cpvsub(a, b)));

	// groove
	cpVect grooveA = cpvadd(body->p, cpvmult(targetNormal, _targetRestLength));
	cpVect grooveB = body->p;

    

	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpGrooveJointNew(attachItem->bodies[0], body, cpBodyWorld2Local(attachItem->bodies[0], grooveA), cpBodyWorld2Local(attachItem->bodies[0], grooveB), cpvzero));
	constraint = cpSpaceAddConstraint(_space, cpDampedSpringNew(attachItem->bodies[0], body, cpBodyWorld2Local(attachItem->bodies[0], grooveA), cpvzero, _targetRestLength, _targetStiffness, _targetDamping));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, attachItem->bodies[0], 0.0f, 0.0f));

	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(a, body->p), cpvsub(b, body->p), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetCollisionType(shape, CollisionTypeTarget);
	cpShapeSetGroup(shape, shapeGroupTargets);
	cpShapeSetUserData(shape, item);

	// switch
	shape = cpSpaceAddShape(_space, cpCircleShapeNew(attachItem->bodies[0], _targetRestLength - _targetSwitchGap, cpBodyWorld2Local(attachItem->bodies[0], grooveA)));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeTargetSwitch);

	item->shapes.push_back(shape);

	item->bodies.push_back(body);

}

void Physics::createDropTarget(LayoutItem *item) {
	cpVect a = cpv(item->v[0].x, item->v[0].y);
	cpVect b = cpv(item->v[1].x, item->v[1].y);
	cpVect mid = cpvmult(cpvadd(a, b), 0.5);

	cpFloat length = cpvdist(a, b);

	cpFloat area = (item->o->r1 * length) * 2;
	cpFloat mass = area * item->o->m->d;

	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForSegment(mass, a, b)));
	cpBodySetPos(body, mid);
	
	// target surface normal;
	cpVect targetNormal = cpvnormalize(cpvperp(cpvsub(a, b)));

	// groove
	cpVect grooveA = cpvadd(body->p, cpvmult(targetNormal, _targetRestLength));
	cpVect grooveB = body->p;

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;

	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpGrooveJointNew(box->bodies[0], body, cpBodyWorld2Local(box->bodies[0], grooveA), cpBodyWorld2Local(box->bodies[0], grooveB), cpvzero));
	constraint = cpSpaceAddConstraint(_space, cpDampedSpringNew(box->bodies[0], body, cpBodyWorld2Local(box->bodies[0], grooveA), cpvzero, _targetRestLength, _targetStiffness, _targetDamping));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, box->bodies[0], 0.0f, 0.0f));

	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(a, body->p), cpvsub(b, body->p), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetCollisionType(shape, CollisionTypeTarget);
	cpShapeSetGroup(shape, shapeGroupTargets);
	cpShapeSetUserData(shape, item);

	// switch
	shape = cpSpaceAddShape(_space, cpCircleShapeNew(box->bodies[0], _targetRestLength - _targetSwitchGap, cpBodyWorld2Local(box->bodies[0], grooveA)));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeTargetSwitch);

	item->shapes.push_back(shape);

	item->bodies.push_back(body);
}

void Physics::createPopbumper(LayoutItem *item) {

	cpFloat area = (item->o->r1 * item->o->r1 * M_PI);
	cpFloat mass = area * item->o->m->d;

	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o->r1, cpvzero)));
	cpBodySetPos(body, cpv(item->v[0].x, item->v[0].y));

	cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o->r1, cpvzero));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetCollisionType(shape, CollisionTypePopbumper);
	cpShapeSetGroup(shape, shapeGroupPopbumpers);
	cpShapeSetUserData(shape, item);

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;

	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(box->bodies[0], body, body->p));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(box->bodies[0], body, 0.0f, 0.0f));

	body->data = item;

	item->bodies.push_back(body);

}

void Physics::activateMech(const char *mechName) {

	LayoutItem *item = &_playfield->getLayout()->find(mechName)->second;

	if (strcmp(item->o->s.c_str(), "flipper") == 0) {
		flip(item);
	}

}

void Physics::deactivateMech(const char *mechName) {

	LayoutItem *item = &_playfield->getLayout()->find(mechName)->second;

	if (strcmp(item->o->s.c_str(), "flipper") == 0) {
		unflip(item);
	}

}

void Physics::flip(LayoutItem *flipper) {

	cpBodyResetForces(flipper->bodies[0]);

	float dir = (float)(flipper->v[0].x < flipper->v[1].x ? 1 : -1);

	// TODO: precompute
	float offset = (float)cpvlength(cpvsub(flipper->v[0], flipper->v[1]));

	cpVect anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, flipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, flipForce * dir), anchor);

	anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(-1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, -flipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, -flipForce * dir), anchor);

}

void Physics::unflip(LayoutItem *flipper) {

	cpBodyResetForces(flipper->bodies[0]);

	float dir = (float)(flipper->v[0].x < flipper->v[1].x ? -1 : 1);

	// TODO: precompute
	float offset = (float)cpvlength(cpvsub(flipper->v[0], flipper->v[1]));

	cpVect anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, unflipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, unflipForce * dir), anchor);

	anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(-1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, -unflipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, -unflipForce * dir), anchor);

}

void Physics::nudge(cpVect dir) {
	cpBodyApplyImpulse(_boxBody, cpv(_nudgeImpulse * dir.x, _nudgeImpulse * dir.y), cpv(0, -0.1));
	//cpBodyApplyForce(_boxBody, cpv(_nudgeImpulse * dir.x, _nudgeImpulse * dir.y), cpv(0, -01.));
}

void Physics::loadConfig() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *configFileName = _bridgeInterface->getScriptPath((const char *)"config.lua");

	int error = luaL_dofile(L, configFileName);
	if (!error) {

        lua_getglobal(L, "config");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *key = lua_tostring(L, -2);
                    
				if (strcmp("timeStep", key) == 0) {
						
                    timeStep = lua_tonumber(L, -1);
                        
				} else if (strcmp("scale", key) == 0) {

					scale = (float)lua_tonumber(L, -1);

				} else if (strcmp("iterations", key) == 0) {

					iterations = (int)lua_tonumber(L, -1);

				}
                    
				lua_pop(L, 1);
			}
            
		}
        
		lua_pop(L, 1); // pop table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}
/*
void Physics::addLayoutItem(layoutItem item) {

	_layoutItems.insert(make_pair(item.n, item));

	for (it_layoutItems it = _layoutItems.begin(); it != _layoutItems.end(); it++) {
		LayoutItem *lprops = &(&*it)->second;
		if (lprops->n == item.n) {
			this->createObject(lprops);
		}
	}

}
*/
void Physics::loadForces() {
    
    lua_State *L = luaL_newstate();
	luaL_openlibs(L);
    
	const char *forcesPath = _bridgeInterface->getScriptPath((const char *)"forces.lua");
    
	int error = luaL_dofile(L, forcesPath);
	if (!error) {
        
        lua_getglobal(L, "forces");
        
		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
					const char *key = lua_tostring(L, -2);
					
					if (strcmp("gravity", key) == 0) {
						
                        // get the first vertex
                        lua_rawgeti(L, -1, 1);
                        gravity.x = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        
                        // get the second vertex
                        lua_rawgeti(L, -1, 2);
                        gravity.y = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                                                
					} else if (strcmp("targetStiffness", key) == 0) {
						_targetStiffness = (float)lua_tonumber(L, -1);
					} else if (strcmp("targetDamping", key) == 0) {
						_targetDamping = (float)lua_tonumber(L, -1);
					} else if (strcmp("targetRestLength", key) == 0) {
						_targetRestLength = (float)lua_tonumber(L, -1);
					} else if (strcmp("targetSwitchGap", key) == 0) {
						_targetSwitchGap = (float)lua_tonumber(L, -1);
					} else if (strcmp("popBumperImpulse", key) == 0) {
						_popBumperImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("popBumperThreshold", key) == 0) {
						_popBumperThreshold = (float)lua_tonumber(L, -1);
					} else if (strcmp("slingshotRestLength", key) == 0) {
						_slingshotRestLength = (float)lua_tonumber(L, -1);
					} else if (strcmp("slingshotSwitchGap", key) == 0) {
						_slingshotSwitchGap = (float)lua_tonumber(L, -1);
					} else if (strcmp("slingshotStiffness", key) == 0) {
						_slingshotStiffness = (float)lua_tonumber(L, -1);
					} else if (strcmp("slingshotDamping", key) == 0) {
						_slingshotDamping = (float)lua_tonumber(L, -1);
					} else if (strcmp("slingshotImpulse", key) == 0) {
						_slingshotImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("flipImpulse", key) == 0) {
						flipImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("flipForce", key) == 0) {
						flipForce = (float)lua_tonumber(L, -1);
					} else if (strcmp("unflipForce", key) == 0) {
						unflipForce = (float)lua_tonumber(L, -1);
					} else if (strcmp("unflipImpulse", key) == 0) {
						unflipImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("nudgeImpulse", key) == 0) {
						_nudgeImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("boxStiffness", key) == 0) {
						_boxStiffness = (float)lua_tonumber(L, -1);
					} else if (strcmp("boxDamping", key) == 0) {
						_boxDamping = (float)lua_tonumber(L, -1);
					}
                    
					lua_pop(L, 1);
				}
            
		}
        
		lua_pop(L, 1); // pop table
        
    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}
    
	lua_close(L);
    
}

void Physics::initCollisionHandlers(void) {
    
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeBall, NULL, __ballPreSolve, NULL, NULL, NULL);
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeSwitch, switchBegin, NULL, NULL, switchSeparate, NULL);
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypePopbumper, NULL, NULL, popBumperPostSolve, NULL, NULL);
    cpSpaceAddCollisionHandler(_space, CollisionTypeTarget, CollisionTypeTargetSwitch, targetSwitchBegin, NULL, NULL, NULL, NULL);
    cpSpaceAddCollisionHandler(_space, CollisionTypeSlingshot, CollisionTypeSlingshotSwitch, slingshotSwitchBegin, NULL, NULL, NULL, NULL);
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeKraken, krakenBegin, NULL, NULL, krakenSeparate, NULL);
    
}

Physics::~Physics(void)
{
	cpSpaceFree(_space);
}

double currentTime;
double accumulator;

void Physics::updatePhysics() {

    double newTime = absoluteTime();
    double fTime = newTime - currentTime;
	if (fTime > 0.25) {
		fTime = 0.25;
	}
	currentTime = newTime;

    accumulator += fTime;
    
    while (accumulator >= timeStep) {
        cpSpaceStep(_space, timeStep);
        accumulator -= timeStep;
    }
    
	//const double alpha = accumulator / timeStep;

}

void Physics::setPaused(bool paused) {
	_paused = paused;
	if (!_paused) {
		currentTime = absoluteTime();
	}
}

bool Physics::getPaused() {
	return _paused;
}

void Physics::resetBallsToInitialPosition() {
	for (it_LayoutItem iterator = _playfield->getLayout()->begin(); iterator != _playfield->getLayout()->end(); iterator++) {
		LayoutItem item = iterator->second;
		if (strcmp("ball", item.o->s.c_str()) == 0) {
			cpBody *body = item.bodies[0];
			cpBodyResetForces(body);
			cpBodySetPos(body, cpv(item.v[0].x, item.v[0].y));
		}
	}
}

