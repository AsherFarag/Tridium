local Time = 0.0
local SpawnRate = 2.0

local function SpawnObstacle()
	go = GameObject.new()
	go:AddMesh()
	go:AddScript("Content/Scripts/Game/Obstacle.lua")

	go:GetTransform().Position = gameObject:GetTransform().Position
end

function OnConstruct()
end

function OnUpdate( deltaTime )
	Time = Time + deltaTime
	if Time >= SpawnRate then
		Time = Time - SpawnRate
		SpawnObstacle()
	end
end

function OnDestroy()
end
