
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

targetStateLit = 0
targetStateUnlit = 1
leftTargetBankStates {}
leftTargetBankTargetCount = 5

leftTargetBankScoreUnlit = 1000
leftTargetBankScoreLit = 100

-- audio defines;
LOOP_INTERVAL_NONE = -1

function handleSwitchClosed(switch)
	if ballInPlay == true or switch = troughSwitch then
	if switch == startButton then
		startButtonPressed()
	elseif switch == troughSwitch then
		troughSwitchClosed()
	elseif switch == target1 then
		leftTargetBankHit(1)
	end
	end
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
		
	else if leftTargetBankStates[index] == targetStateLit then
		score = score + leftTargetBankScoreLit
		playSound(chimeC0, LOOP_INTERVAL_NONE)
	end
end

function handleSwitchOpened(switch)
	if switch == troughSwitch then
		--just an example
	end
end

function troughSwitchClosed()
	if ballInPlay == false then
		ballInPlay = true -- unnecessary?
	end
end

function startButtonPressed()
	if gameInProgress == false then
		startGame()
	end
end

function startGame()
	resetAll()
	serveBallToTrough()
	playSound(gameStartMusic, 10)
	gameInProgress = true
end

function playSound(sound, loopIntervalSeconds)
	-- C API stub
end

function resetAll()
	resetScore()
	resetAudio()
	resetMechs()
	resetLights()
end

function resetScore()
	score = startingScore
	bonus = startingBonus
	scoreMult = startingScoreMult
	bonusMult = startingBonusMult
end

function resetAudio()
	--C API stub
end

function resetMechs()
	--C API stub
end

function resetLights() {
	--C API stub
}

function serveBallToTrough() {
	--C API stub
}

