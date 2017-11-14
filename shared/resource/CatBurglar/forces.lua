
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

	slingshotRestLength = 0.3,
	slingshotSwitchGap = 0.3,
	slingshotStiffness = 13.0,
	slingshotDamping = 0.1,
	slingshotImpulse = 0.13,

	targetStiffness = 190, -- glass == 90
	targetDamping = 0.10,
	targetRestLength = 0.3,
	targetSwitchGap = 0.05,

	popBumperThreshold = 0,
	popBumperImpulse = 0.025,

	nudgeImpulse = 30

}

