
objects = {
	ball = {
		s = "ball", -- "ball" shape defined internally - is in shape group "ballShapeGroup"
		m = "ball",
		r1 = 0.5
	},
	post = {
		s = "circle",
		m = "rubber",
		r1 = 0.25
	},
	rubber = {
		s = "segment",
		m = "rubber",
		r1 = 0.125
	},
	wireGuide = {
		s = "segment",
		m = "wire",
		r1 = 0.125
	},
	woodGuide = {
		s = "segment",
		m = "wood",
		r1 = 0.125
	},
	plastic = {
		s = "poly",
		m = "plastic"
	},
	box = {
		s = "box", -- "box" shape defined internally - internal winding is clockwise from blh
		m = "wood",
		r1 = 0.25
	},
	flipper = {
		s = "flipper", -- "flipper" shape defined internally
		m = "flipper",
		r1 = 0.45,
		r2 = 0.35
	},
	slingshot = {
		s = "slingshot", -- "slingshot" shape defined internally
		m = "rubber",
		r1 = 0.125
	},
	target = {
		s = "target", -- "target" shape defined internally
		m = "plastic",
		r1 = 0.175
	},
	switch = {
		s = "switch", -- "switch" shape defined internally (sensor)
		r1 = 0.05
	},
	popbumper = {
		s = "popbumper", -- "popbumper" shape defined internally
		m = "rubber",
		r1 = 1.1
	},
	roundLamp = {
		s = "roundLamp", -- "roundLamp" shape defined internally
		r1 = 0.5
	},
	roundLampSmall = {
		s = "roundLamp",
		r1 = 0.25
	},
	arrowLamp = {
		s = "arrowLamp", -- "arrowLamp" shape defined internally
		r1 = 0.25,
		r2 = 0.17
	},
	notch = {
		s = "notch", -- "notch" shape defined internally
		r1 = 0.125
	}
}

