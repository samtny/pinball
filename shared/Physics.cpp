
#include "Physics.h"

#include "PinballBridgeInterface.h"

#include "PhysicsDelegate.h"

#include "Parts.h"

#include "Util.h"

#include "chipmunk/chipmunk.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

static Physics *physics_currentInstance;

static cpSpace *_space;

static cpVect gravity = cpv(0.0, 9.80665f);

static float flipImpulse = 0.02f;
static float unflipImpulse = 0.02f;

static float flipForce = 0.2f;
static float unflipForce = 0.2f;

static float _targetRestLength = 0;
static float _targetSwitchGap = 0;
static float _targetStiffness = 0;
static float _targetDamping = 0;

static float _slingshotRestLength = 0.6;
static float _slingshotSwitchGap = 0.25;
static float _slingshotStiffness = 35.0;
static float _slingshotDamping = 0.05;
static float _slingshotImpulse = 0.02;

static float _popBumperThreshold = 0.005;
static float _popBumperImpulse = 0.04;

static double timeStep = 1.0/180.0;

static int iterations = 10;

static float scale = 37;

enum shapeGroup {
	shapeGroupBox,
    shapeGroupBall,
	shapeGroupFlippers,
	shapeGroupTargets,
	shapeGroupPopbumpers,
	shapeGroupSlingshots
};

enum CollisionType {
	CollisionTypeNone,
	CollisionTypeSwitch,
	CollisionTypeBall,
	CollisionTypeTarget,
	CollisionTypeTargetSwitch,
	CollisionTypePopbumper,
	CollisionTypeSlingshot,
	CollisionTypeSlingshotSwitch
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

map<string, layoutItem> *Physics::getLayoutItems() {
	return &_layoutItems;
}

void Physics::init() {

	this->loadConfig();
	this->loadMaterials();
	this->loadObjects();
	this->loadLayout();
    this->loadForces();

	_space = cpSpaceNew();
	cpSpaceSetIterations(_space, iterations);
	
	// TODO: move to properties of item/object;
	_targetRestLength *= 1 / scale;
	_targetSwitchGap *= 1 / scale;
	_slingshotRestLength *= 1 / scale;
	_slingshotSwitchGap *= 1 / scale;

	for (it_layoutItems iterator = _layoutItems.begin(); iterator != _layoutItems.end(); iterator++) {
		layoutItem *lprops = &(&*iterator)->second;
		this->applyScale(lprops);
		this->createObject(lprops);
	}

	/*
	for (it_layoutItems iterator = layoutItems.begin(); iterator != layoutItems.end(); iterator++) {
		layoutItem lprops = iterator->second;
		this->applyScale(&lprops);
		this->createObject(&lprops);
	}
	*/

	this->initCollisionHandlers();

	cpSpaceSetGravity(_space, gravity);

}

static int __ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	return physics_currentInstance->ballPreSolve(arb, space, unused);

}

int Physics::ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	materialProperties *mat = &_materials["steel"];

	arb->e = mat->e;
	arb->u = mat->f;

    _bridgeInterface->playSound((void *)"flip");
    
	return 1;

}

static int switchBegin(cpArbiter *arb, cpSpace *space, void *unused) {

	cpShape *a;
	cpShape *b;
	cpArbiterGetShapes(arb, &a, &b);

	layoutItem *l = (layoutItem *)b->data;

	physics_currentInstance->getDelegate()->switchClosed(l->n.c_str());

	return 1;

}

static void switchSeparate(cpArbiter *arb, cpSpace *space, void *unused) {

	cpShape *a;
	cpShape *b;
	cpArbiterGetShapes(arb, &a, &b);

	layoutItem *l = (layoutItem *)b->data;

	physics_currentInstance->getDelegate()->switchOpened(l->n.c_str());

}

static void popBumperPostSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	if (!cpArbiterIsFirstContact(arb)) return;

	cpVect impulse = cpArbiterTotalImpulse(arb);
	
	if (cpvlength(impulse) >= _popBumperThreshold) {
		
		cpBody *ball, *pop;
		cpArbiterGetBodies(arb, &ball, &pop);

		cpVect normal = cpArbiterGetNormal(arb, 0);

		cpBodyApplyImpulse(ball, cpvmult(normal, -_popBumperImpulse), cpvzero);
		
		layoutItem *l = (layoutItem *)pop->data;

		physics_currentInstance->getDelegate()->switchClosed(l->n.c_str());

	}

}

static int targetSwitchBegin(cpArbiter *arb, cpSpace *space, void *unused) {

	cpShape *target, *sw;
	cpArbiterGetShapes(arb, &target, &sw);

	layoutItem *l = (layoutItem *)target->data;

	physics_currentInstance->getDelegate()->switchClosed(l->n.c_str());

	// we don't need more information from this
	return 0;

}

static int slingshotSwitchBegin(cpArbiter *arb, cpSpace *space, void *unused) {

	cpBody *slingshot, *sw;
	cpArbiterGetBodies(arb, &slingshot, &sw);

	cpVect normal = cpArbiterGetNormal(arb, 0);

	cpBodyApplyImpulse(slingshot, cpvmult(normal, -_slingshotImpulse), cpvzero);
		
	/*
	layoutItem *l = (layoutItem *)slingshot->data;

	physics_currentInstance->getDelegate()->switchClosed(l->n.c_str());
	*/
	return 1;

}

void Physics::initCollisionHandlers(void) {

	cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeBall, NULL, __ballPreSolve, NULL, NULL, NULL);
	cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeSwitch, switchBegin, NULL, NULL, switchSeparate, NULL);
	cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypePopbumper, NULL, NULL, popBumperPostSolve, NULL, NULL);
	cpSpaceAddCollisionHandler(_space, CollisionTypeTarget, CollisionTypeTargetSwitch, targetSwitchBegin, NULL, NULL, NULL, NULL);
	cpSpaceAddCollisionHandler(_space, CollisionTypeSlingshot, CollisionTypeSlingshotSwitch, slingshotSwitchBegin, NULL, NULL, NULL, NULL);

}

cpSpace *Physics::getSpace() {
	return _space;
}

void Physics::applyScale(layoutItem *iprops) {

	for (int i = 0; i < iprops->count; i++) {
		iprops->v[i].x *= 1 / scale;
		iprops->v[i].y *= 1 / scale;
	}

	float localScale = scale * 1 / iprops->s;

	iprops->o.r1 *= 1 / localScale;
	iprops->o.r2 *= 1 / localScale;
	
	//iprops->o.t.s *= 1 / scale;
	
}

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

void destroyObject(cpSpace *space, void *itm, void *unused) {

	layoutItem *item = (layoutItem *)itm;

	layoutItem box = physics_currentInstance->getLayoutItems()->find("box")->second;

	if (item->body && (strcmp(item->n.c_str(), "box") == 0 || item->body != box.body)) {
		cpBodyEachConstraint(item->body, destroyConstraint, NULL);
		cpBodyEachShape(item->body, destroyShape, NULL);
		cpSpaceRemoveBody(space, item->body);
		cpBodyFree(item->body);
	} else if (item->shape) {
		cpSpaceRemoveShape(space, item->shape);
		cpShapeFree(item->shape);
	}

}

void Physics::destroyObject(layoutItem *item) {

	if (cpSpaceIsLocked(_space)) {
		cpSpaceAddPostStepCallback(_space, ::destroyObject, item, NULL);
	} else {
		::destroyObject(_space, item, NULL);
	}

}

void Physics::createObject(layoutItem *layoutItem) {
	
	if (strcmp(layoutItem->o.s.c_str(), "box") == 0) {
		layoutItem->body = this->createBox(layoutItem);
		layoutItem->width = (float)(layoutItem->v[3].x - layoutItem->v[0].x);
		layoutItem->height = (float)(layoutItem->v[1].y - layoutItem->v[0].y);
	} else if (strcmp(layoutItem->o.s.c_str(), "segment") == 0) {
		this->createSegment(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "flipper") == 0) {
		layoutItem->body = this->createFlipper(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "ball") == 0) {
        layoutItem->body = this->createBall(layoutItem);
    } else if (strcmp(layoutItem->o.s.c_str(), "switch") == 0) {
		this->createSwitch(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "circle") == 0) {
		this->createCircle(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "target") == 0) {
		layoutItem->body = this->createTarget(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "popbumper") == 0) {
		layoutItem->body = this->createPopbumper(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "slingshot") == 0) {
		layoutItem->body = this->createSlingshot(layoutItem);
	}

}

cpBody *Physics::createBox(layoutItem *item) {

	cpBody *body, *staticBody = cpSpaceGetStaticBody(_space);
	cpShape *shape;
	cpConstraint *constraint;

	cpFloat area = (item->v[1].y - item->v[0].y) * (item->v[3].x - item->v[0].x);
	cpFloat mass = area * item->o.m.d;

	// create body on which to hang the "box";
	body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForPoly(mass, 4, item->v, cpvzero)));

	// the implications of attaching all playfield objects to this non-zero-indexed body are not inconsequential...
	cpBodySetPos(body, cpvmult(cpvadd(item->v[2], item->v[0]), 0.5f));
	
	// pin the box body at the four corners;
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[0]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[1]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[2]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[3]));
	
	// pin the box in place;
	constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, cpvmult(cpvadd(item->v[0], item->v[2]), 0.5)) );
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, staticBody, 0.0f, 0.0f));
	
	// hang the box shapes on the body;
	// left
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[0]), cpBodyWorld2Local(body, item->v[1]), item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// top
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[1]), cpBodyWorld2Local(body, item->v[2]), item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// right
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[2]), cpBodyWorld2Local(body, item->v[3]), item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// bottom
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[3]), cpBodyWorld2Local(body, item->v[0]), item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	cpBodySetUserData(body, item);

	return body;

}

static int ballCollisionGroup = 2048;

cpBody *Physics::createBall(layoutItem *item) {
    
    cpFloat area = (item->o.r1 * item->o.r1 * M_PI);
    cpFloat mass = area * item->o.m.d;
    
    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o.r1, cpvzero)));
    cpBodySetPos(body, item->v[0]);
    
    cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o.r1, cpvzero));
    cpShapeSetElasticity(shape, item->o.m.e);
    cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBall + ballCollisionGroup); // TODO: ball shape group is kludged
	cpShapeSetCollisionType(shape, CollisionTypeBall);

	body->data = item;

	ballCollisionGroup++;

	return body;

}

cpBody *Physics::createFlipper(layoutItem *item) {

	cpFloat area = (item->o.r1 * M_PI) * 2; // approx
	cpFloat mass = area * item->o.m.d;

	cpFloat direction = item->v[0].x <= item->v[1].x ? -1 : 1; // rotate clockwise for right-facing flipper...

	cpFloat length = cpvdist(item->v[0], item->v[1]);
	cpFloat flipAngle = direction * cpfacos(cpvdot(cpvnormalize(cpvsub(item->v[1],item->v[0])), cpvnormalize(cpvsub(item->v[2],item->v[0]))));

	cpFloat flipStart = flipAngle > 0 ? 0 : flipAngle;
	cpFloat flipEnd = flipAngle > 0 ? flipAngle : 0;

	// flipper body is round centered at base of flipper, and for this implementation has radius == flipper length;
	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, length, cpvzero)));
	cpBodySetPos(body, item->v[0]);

	layoutItem *box = &_layoutItems.find("box")->second;

 	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, box->body, item->v[0]));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, box->body, flipStart, flipEnd));

	// lflipper base shape
	cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o.r1, cpvzero));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupFlippers);

	float diff = item->o.r1 - item->o.r2; // r1-prime
	float loft = atan2f(diff, length);
	float facelen = diff / sin(loft);
	cpVect p2p1 = cpvsub(item->v[0], item->v[1]);
	cpVect p2p1n = cpvnormalize(p2p1);
	cpVect p3n = cpvrotate(p2p1n, cpvforangle(loft));
	cpVect p3 = cpvadd(item->v[1], cpvmult(p3n, facelen));

	// lflipper face shapes
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(item->v[1], body->p), cpvsub(p3, body->p), item->o.r2));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupFlippers);

	loft = -atan2f(diff, length);
	p3n = cpvrotate(p2p1n, cpvforangle(loft));
	p3 = cpvadd(item->v[1], cpvmult(p3n, facelen));

	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(item->v[1], body->p), cpvsub(p3, body->p), item->o.r2));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupFlippers);

	cpBodySetUserData(body, item);

	return body;

}

void Physics::createSwitch(layoutItem *item) {

	layoutItem *box = &_layoutItems.find("box")->second;

	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(box->body, cpBodyWorld2Local(box->body, item->v[0]), cpBodyWorld2Local(box->body, item->v[1]), item->o.r1));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeSwitch);
	cpShapeSetUserData(shape, item);

	item->shape = shape;

	item->body = box->body;
	
}

void Physics::createSegment(layoutItem *item) {

	layoutItem *box = &_layoutItems.find("box")->second;

	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(box->body, cpBodyWorld2Local(box->body, item->v[0]), cpBodyWorld2Local(box->body, item->v[1]), item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetUserData(shape, item);

	item->shape = shape;

	item->body = box->body;

}

cpBody *Physics::createSlingshot(layoutItem *item) {

	cpVect mid = cpvmult(cpvadd(item->v[0], item->v[1]), 0.5);

	cpFloat area = (item->o.r1 * M_PI) * 2; // approx
	cpFloat mass = area * item->o.m.d;

	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForSegment(mass, item->v[0], item->v[1])));
	cpBodySetPos(body, mid);

	layoutItem *box = &_layoutItems.find("box")->second;

 	// target surface normal;
	cpVect normal = cpvnormalize(cpvperp(cpvsub(item->v[0], item->v[1])));

	// groove
	cpVect grooveA = cpvadd(body->p, cpvmult(normal, _slingshotRestLength));
	cpVect grooveB = body->p;

	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpGrooveJointNew(box->body, body, cpBodyWorld2Local(box->body, grooveA), cpBodyWorld2Local(box->body, grooveB), cpvzero));
	constraint = cpSpaceAddConstraint(_space, cpDampedSpringNew(box->body, body, cpBodyWorld2Local(box->body, grooveA), cpvzero, _slingshotRestLength, _slingshotStiffness, _slingshotDamping));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, box->body, 0.0f, 0.0f));

	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(item->v[0], body->p), cpvsub(item->v[1], body->p), item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupSlingshots);
	cpShapeSetCollisionType(shape, CollisionTypeSlingshot);
	cpShapeSetUserData(shape, item);

	// switch
	shape = cpSpaceAddShape(_space, cpCircleShapeNew(box->body, _slingshotRestLength - _slingshotSwitchGap, cpBodyWorld2Local(box->body, grooveA)));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeSlingshotSwitch);

	item->shape = shape;

	return body;

}

void Physics::createCircle(layoutItem *item) {

	layoutItem *box = &_layoutItems.find("box")->second;

	cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(box->body, item->o.r1, cpBodyWorld2Local(box->body, item->v[0])));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetUserData(shape, item);

	item->shape = shape;

	item->body = box->body;

}

cpBody *Physics::createTarget(layoutItem *item) {

	cpVect a = cpv(item->v[0].x, item->v[0].y);
	cpVect b = cpv(item->v[1].x, item->v[1].y);
	cpVect mid = cpvmult(cpvadd(a, b), 0.5);

	cpFloat length = cpvdist(a, b);

	cpFloat area = (item->o.r1 * length) * 2;
	cpFloat mass = area * item->o.m.d;

	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForSegment(mass, a, b)));
	cpBodySetPos(body, mid);
	
	// target surface normal;
	cpVect targetNormal = cpvnormalize(cpvperp(cpvsub(a, b)));

	// groove
	cpVect grooveA = cpvadd(body->p, cpvmult(targetNormal, _targetRestLength));
	cpVect grooveB = body->p;

	layoutItem *box = &_layoutItems.find("box")->second;

	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpGrooveJointNew(box->body, body, cpBodyWorld2Local(box->body, grooveA), cpBodyWorld2Local(box->body, grooveB), cpvzero));
	constraint = cpSpaceAddConstraint(_space, cpDampedSpringNew(box->body, body, cpBodyWorld2Local(box->body, grooveA), cpvzero, _targetRestLength, _targetStiffness, _targetDamping));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, box->body, 0.0f, 0.0f));

	cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpvsub(a, body->p), cpvsub(b, body->p), item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetCollisionType(shape, CollisionTypeTarget);
	cpShapeSetGroup(shape, shapeGroupTargets);
	cpShapeSetUserData(shape, item);

	// switch
	shape = cpSpaceAddShape(_space, cpCircleShapeNew(box->body, _targetRestLength - _targetSwitchGap, cpBodyWorld2Local(box->body, grooveA)));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeTargetSwitch);

	item->shape = shape;

	return body;

}

cpBody *Physics::createPopbumper(layoutItem *item) {

	cpFloat area = (item->o.r1 * item->o.r1 * M_PI);
	cpFloat mass = area * item->o.m.d;

	cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o.r1, cpvzero)));
	cpBodySetPos(body, cpv(item->v[0].x, item->v[0].y));

	cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o.r1, cpvzero));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetCollisionType(shape, CollisionTypePopbumper);
	cpShapeSetGroup(shape, shapeGroupPopbumpers);
	cpShapeSetUserData(shape, item);

	layoutItem *box = &_layoutItems.find("box")->second;

	cpConstraint *constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(box->body, body, body->p));
	constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(box->body, body, 0.0f, 0.0f));

	body->data = item;

	return body;

}

void Physics::activateMech(const char *mechName) {

	layoutItem item = _layoutItems.find(mechName)->second;

	if (strcmp(item.o.s.c_str(), "flipper") == 0) {
		flip(&item);
	}

}

void Physics::deactivateMech(const char *mechName) {

	layoutItem item = _layoutItems.find(mechName)->second;

	if (strcmp(item.o.s.c_str(), "flipper") == 0) {
		unflip(&item);
	}

}

void Physics::flip(layoutItem *flipper) {

	cpBodyResetForces(flipper->body);

	float dir = flipper->v[0].x < flipper->v[1].x ? 1 : -1;

	// TODO: precompute
	float offset = cpvlength(cpvsub(flipper->v[0], flipper->v[1]));

	cpVect anchor = cpvadd(flipper->body->p, cpvmult(cpv(1, 0), offset));

	cpBodyApplyImpulse(flipper->body, cpv(0, flipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->body, cpv(0, flipForce * dir), anchor);

	anchor = cpvadd(flipper->body->p, cpvmult(cpv(-1, 0), offset));

	cpBodyApplyImpulse(flipper->body, cpv(0, -flipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->body, cpv(0, -flipForce * dir), anchor);

}

void Physics::unflip(layoutItem *flipper) {

	cpBodyResetForces(flipper->body);

	float dir = flipper->v[0].x < flipper->v[1].x ? -1 : 1;

	// TODO: precompute
	float offset = cpvlength(cpvsub(flipper->v[0], flipper->v[1]));

	cpVect anchor = cpvadd(flipper->body->p, cpvmult(cpv(1, 0), offset));

	cpBodyApplyImpulse(flipper->body, cpv(0, unflipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->body, cpv(0, unflipForce * dir), anchor);

	anchor = cpvadd(flipper->body->p, cpvmult(cpv(-1, 0), offset));

	cpBodyApplyImpulse(flipper->body, cpv(0, -unflipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->body, cpv(0, -unflipForce * dir), anchor);

}

void Physics::loadConfig() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *configFileName = _bridgeInterface->getPathForScriptFileName((void *)"config.lua");

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

void Physics::loadMaterials() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *materialsFileName = _bridgeInterface->getPathForScriptFileName((void *)"materials.lua");

	int error = luaL_dofile(L, materialsFileName);
	if (!error) {

        lua_getglobal(L, "materials");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				materialProperties props = { "", -1, -1, -1 };

				// "value" is properties table;
				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {

					// property name
					const char *key = lua_tostring(L, -2);

					// property value
					float val = (float)lua_tonumber(L, -1);

					if (strcmp("e", key) == 0) {
						props.e = val;
					} else if (strcmp("f", key) == 0) {
						props.f = val;
					} else if (strcmp("d", key) == 0) {
						props.d = val;
					}

					lua_pop(L, 1);
				}
				
				_materials.insert(make_pair(name, props));

				lua_pop(L, 1);
			}

		}

		lua_pop(L, 1); // pop materials table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Physics::loadObjects() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *objectsPath = _bridgeInterface->getPathForScriptFileName((void *)"objects.lua");

	int error = luaL_dofile(L, objectsPath);
	if (!error) {

        lua_getglobal(L, "objects");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				objectProperties props = { name, -1, "", -1, -1, { "", -1, -1, -1, }, { "", -1, -1, -1, -1 } };

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("s", key) == 0) {
						props.s = lua_tostring(L, -1);
					} else if (strcmp("m", key) == 0) {
						props.m = _materials[lua_tostring(L, -1)];
					} else if (strcmp("r1", key) == 0) {
						props.r1 = (float)lua_tonumber(L, -1);
					} else if (strcmp("r2", key) == 0) {
						props.r2 = (float)lua_tonumber(L, -1);
					} else if (strcmp("t", key) == 0) {

						lua_pushnil(L);
						while(lua_next(L, -2) != 0) {
							
							const char *tkey = lua_tostring(L, -2);

							if (strcmp("n", tkey) == 0) {
								props.t.n = lua_tostring(L, -1);
							} else if (strcmp("x", tkey) == 0) {
								props.t.x = (int)lua_tonumber(L, -1);
							} else if (strcmp("y", tkey) == 0) {
								props.t.y = (int)lua_tonumber(L, -1);
							} else if (strcmp("w", tkey) == 0) {
								props.t.w = (int)lua_tonumber(L, -1);
							} else if (strcmp("h", tkey) == 0) {
								props.t.h = (int)lua_tonumber(L, -1);
							} else if (strcmp("a", tkey) == 0) {
								props.t.a = (float)lua_tonumber(L, -1);
							}

							lua_pop(L, 1);

						}

					}

					lua_pop(L, 1);
				}
				
				_objects.insert(make_pair(name, props));

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

void Physics::loadLayout() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *layoutPath = _bridgeInterface->getPathForScriptFileName((void *)"layout.lua");

	int error = luaL_dofile(L, layoutPath);
	if (!error) {

        lua_getglobal(L, "layout");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				layoutItem props = { name };
				props.s = -1;
				props.editing = false;

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("o", key) == 0) {
						
						props.o = _objects[lua_tostring(L, -1)];

					} else if (strcmp("v", key) == 0) {
						
						int length = lua_rawlen(L, -1);
						
						// traverse 2d vects
						for (int i = 1; i <= length; i++)
						{

							// init vect object
							cpVect v;

							// get the 2d table
							lua_rawgeti(L, -1, i);

							// get the first vertex
							lua_rawgeti(L, -1, 1);
							v.x = (float)lua_tonumber(L, -1);
							lua_pop(L, 1);

							// get the second vertex
							lua_rawgeti(L, -1, 2);
							v.y = (float)lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							// pop the table;
							lua_pop(L, 1);
							
							// assign vect to array
							props.v[i-1] = v;

						}

						props.count = length;

					} else if (strcmp("s", key) == 0) {
						props.s = (float)lua_tonumber(L, -1);
					}

					lua_pop(L, 1);
				}
				
				if (props.s == -1) {
					props.s = 1;
				}

				_layoutItems.insert(make_pair(name, props));

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

void Physics::addLayoutItem(layoutItem item) {

	_layoutItems.insert(make_pair(item.n, item));

	for (it_layoutItems it = _layoutItems.begin(); it != _layoutItems.end(); it++) {
		layoutItem *lprops = &(&*it)->second;
		if (lprops->n == item.n) {
			this->createObject(lprops);
		}
	}

}

void Physics::loadForces() {
    
    lua_State *L = luaL_newstate();
	luaL_openlibs(L);
    
	const char *forcesPath = _bridgeInterface->getPathForScriptFileName((void *)"forces.lua");
    
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
	for (it_layoutItems iterator = _layoutItems.begin(); iterator != _layoutItems.end(); iterator++) {
		layoutItem item = iterator->second;
		if (strcmp("ball", item.o.s.c_str()) == 0) {
			cpBody *body = item.body;
			cpBodyResetForces(body);
			cpBodySetPos(body, cpv(item.v[0].x, item.v[0].y));
		}
	}
}