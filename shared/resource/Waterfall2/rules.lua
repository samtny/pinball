
gameInProgress = false

startingScore = 0
startingBonus = 0
startingScoreMult = 1
startingBonusMult = 1

score = startingScore
bonus = startingBonus
scoreMult = startingScoreMult
bonusMult = startingBonusMult

scoreMultMax = 10
bonusMultMax = 10

bonusMultIncrement = 1.0

switches = {}

waterfallSwitchValue = 100

popBumperValue = 10

targetValue = 201

targetStateLit = 0
targetStateUnlit = 1
leftTargetBankStates = {0,0,0,0,0}
leftTargetBankTargetCount = 5

leftTargetBankScoreUnlit = 1000
leftTargetBankScoreLit = 100

ballsInPlay = 0

gameRestartDelay = 5

-- audio defines;
LOOP_INTERVAL_NONE = -1

function handleSwitchClosed(switch)
	print "handleSwitchClosed"
	print (switch)
	if ballInPlay == true or switch == troughSwitch or switch == "troughSwitch" then -- TODO: decide int vs. strings plzz...
		if switch == troughSwitch or switch == "troughSwitch" then
			troughSwitchClosed()
		elseif switch == target1 then
			leftTargetBankHit(1)
		end
	elseif switch == "startButton" then
		startButtonPressed()
	elseif switch == "waterfallSwitch0" or switch == "waterfallSwitch1" then
		addToScore(waterfallSwitchValue)
	elseif switch == "pop0" then
		addToScore(popBumperValue)
	elseif switch == "target0" or switch == "target1" then
		addToScore(targetValue)
	end
end

function addToScore(points)
	score = score + points
	updateOverlayText("score", score)
end

function updateOverlayText(key, val)
	-- C API stub
	print (val)
end

function leftTargetBankHit(index)
	
	if leftTargetBankStates[index] == targetStateUnlit then
		
		score = score + leftTargetBankScoreUnlit
		
		local targetsLit = 0
		for i = 1, leftTargetBankTargetCount do
			if leftTargetBankStates[i] == targetStateLit then
				targetsLit = targetsLit + 1
			end
		end
		if targetsLit == leftTargetBankTargetCount then
			if bonusMult < bonusMultMax then
				bonusMult = bonusMult + bonusMultIncrement
				for i = 1, leftTargetBankTargetCount do
					leftTargetBankStates[i] = targetStateUnlit
				end
			end
		end
		
		playSound(chimeC1, LOOP_INTERVAL_NONE)
		
	elseif leftTargetBankStates[index] == targetStateLit then
		
		score = score + leftTargetBankScoreLit
		playSound(chimeC0, LOOP_INTERVAL_NONE)

	end

end

function handleSwitchOpened(switch)
	if switch == "troughSwitch" then
		troughSwitchOpened()
	end
end

function troughSwitchClosed()
	print "troughSwitchClosed"
	if gameInProgress == true then
		ballsInPlay = ballsInPlay - 1
		print (ballsInPlay)
		if ballsInPlay <= 0 then
			endGame()
		end
	end
end

function troughSwitchOpened()
	print "troughSwitchOpened"
	if gameInProgress == true then
		ballsInPlay = ballsInPlay + 1
	end
end

function startButtonPressed()
	
	print "startButtonPressed"
	
	if gameInProgress == false then
		startGame()
	end

end

function addTimer(duration, funcName, parameter)
	-- C API stub
end

function startGame()

	print "startGame"

	-- TODO: switch on __PINBALL_DEBUG flag;
	resetBallPosition()
	setCameraMode("centered")
	doCameraEffect("swirlin")

	resetAll()
	serveBallToTrough()
	playSound(gameStartMusic, 10)

	ballsInPlay = 0 -- yeah, not how this works...

	gameInProgress = true

end

function endGame()
	print "endGame"

	gameInProgress = false

	addTimer(gameRestartDelay, "startGame", nil)
end

function playSound(sound, loopIntervalSeconds)
	-- C API stub
end

function doCameraEffect(effectName)
	-- C API stub
end

function resetAll()
	resetScore()
	resetAudio()
	resetMechs()
	resetLights()
	resetCamera()
end

function resetScore()
	score = startingScore
	updateOverlayText("score", score)
	bonus = startingBonus
	scoreMult = startingScoreMult
	bonusMult = startingBonusMult
end

function resetBallPosition()
	-- C API stub
end

function resetAudio()
	--C API stub
end

function resetMechs()
	--C API stub
end

function resetLights()
	--C API stub
end

function resetCamera()
	setCameraMode("centered")
end

function serveBallToTrough()
	--C API stub
end

function setCameraFollowsBall()
	--C API stub
end

function setCameraMode(modeName)
	-- C API stub
end

