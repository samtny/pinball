
forces = {

	gravityBase = { 0.0, -9.80665 },
	tableAngle = 6.5,
	gravity = { 0.0, -9.80665 * ( 6.5 / 90.0 ) },

	flipImpulse = 0.001,
	flipForce = 0.8,
	unflipImpulse = 0.001,
	unflipForce = 0.2,

	slingshotRestLength = 0.6,
	slingshotSwitchGap = 0.25,
	slingshotStiffness = 15.0,
	slingshotDamping = 0.9,
	slingshotImpulse = 0.04,

	targetStiffness = 40.0, -- glass == 90
	targetDamping = 0.15,
	targetRestLength = 0.5,
	targetSwitchGap = 0.25,

	popBumperThreshold = 0.005,
	popBumperImpulse = 0.0055

}

