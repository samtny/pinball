
forces = {

	gravityBase = { 0.0, -9.80665 },
	tableAngle = 6.5,
	gravity = { 0.0, -9.80665 * math.sin(6.5 * (6.5 / 180)) },
	
	boxStiffness = 10000, -- steel = 200
	boxDamping = 1000,

	flipImpulse = 0.13,
	flipForce = 8,

	unflipImpulse = 0.1,
	unflipForce = 3,

	nudgeImpulse = 30

}

