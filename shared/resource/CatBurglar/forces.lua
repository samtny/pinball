
forces = {

	gravityBase = { 0.0, -9.80665 },
	tableAngle = 6.5,
	gravity = { 0.0, -9.80665 * ( 16.5 / 90.0 ) },
	
	boxStiffness = 10000, -- steel = 200
	boxDamping = 1000,

	flipImpulse = 0.13,
	flipForce = 8,

	unflipImpulse = 0.1,
	unflipForce = .25,

	slingshotRestLength = 0.4,
	slingshotSwitchGap = 0.3,
	slingshotStiffness = 15.0,
	slingshotDamping = 0.5,
	slingshotImpulse = 0.07,

	targetStiffness = 190, -- glass == 90
	targetDamping = 0.10,
	targetRestLength = 0.3,
	targetSwitchGap = 0.05,

	popBumperThreshold = 0,
	popBumperImpulse = 0.025,

	nudgeImpulse = 30

}

