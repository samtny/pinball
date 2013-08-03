
forces = {

	gravityBase = { 0.0, -9.80665 },
	tableAngle = 6.5,
	gravity = { 0.0, -9.80665 * ( 3.5 / 90.0 ) },
	
	boxStiffness = 20.0, -- steel = 200
	boxDamping = 5.0,

	flipImpulse = 0.011,
	flipForce = 0.25,
	unflipImpulse = 0.005,
	unflipForce = 0.05,

	slingshotRestLength = 0.6,
	slingshotSwitchGap = 0.35,
	slingshotStiffness = 15.0,
	slingshotDamping = 0.5,
	slingshotImpulse = 0.007,

	targetStiffness = 10.0, -- glass == 90
	targetDamping = 0.50,
	targetRestLength = 0.125,
	targetSwitchGap = 0.05,

	popBumperThreshold = 0.005,
	popBumperImpulse = 10.1055,

	nudgeImpulse = 0.1

}

