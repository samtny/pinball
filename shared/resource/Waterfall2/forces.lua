
forces = {

	gravityBase = { 0.0, -9.80665 },
	tableAngle = 6.5,
	gravity = { 0.0, -9.80665 * ( 6.5 / 90.0 ) },

	flipImpulse = 1.0,
	flipForce = 0.5,
	unflipImpulse = 1.0,
	unflipForce = 0.5,

	slingshotRestLength = 0.6,
	slingshotSwitchGap = 0.25,
	slingshotStiffness = 15.0,
	slingshotDamping = 0.35,
	slingshotImpulse = 0.05,

	targetStiffness = 35.0, -- glass == 90
	targetDamping = 0.15,
	targetRestLength = 0.5,
	targetSwitchGap = 0.25,

	popBumperThreshold = 0.005,
	popBumperImpulse = 0.02

}

