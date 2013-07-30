
forces = {

	gravityBase = { 0.0, -9.80665 },
	tableAngle = 6.5,
	gravity = { 0.0, -9.80665 * ( 10.5 / 90.0 ) },
	
	boxStiffness = 500.0, -- steel = 200
	boxDamping = 70.0,

	flipImpulse = 0.005,
	flipForce = 0.02,
	unflipImpulse = 0.005,
	unflipForce = 0.02,

	slingshotRestLength = 0.6,
	slingshotSwitchGap = 0.35,
	slingshotStiffness = 55.0,
	slingshotDamping = 0.5,
	slingshotImpulse = 0.007,

	targetStiffness = 190.0, -- glass == 90
	targetDamping = 0.50,
	targetRestLength = 0.9,
	targetSwitchGap = 0.25,

	popBumperThreshold = 0.005,
	popBumperImpulse = 0.0055,

	nudgeImpulse = 0.1

}

