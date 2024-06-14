local Time = 0.0
local SpawnTimer = 0.0
local SpawnRate = 0.0

local function SpawnObstacle()
	go = GameObject.new()
	--go:AddMesh()
	go:AddSprite("Content/Engine/Editor/Icons/Alpha.png")
	go:AddScript("Content/Scripts/Game/Obstacle.lua")

	go:GetTransform().Position = gameObject:GetTransform().Position
end

function OnConstruct()
end

function OnUpdate( deltaTime )
	Time = Time + deltaTime
	gameObject:GetTransform().Position.x = math.sin(Time)

	SpawnTimer = SpawnTimer + deltaTime
	if SpawnTimer >= SpawnRate then
		SpawnTimer = SpawnTimer - SpawnRate
		SpawnObstacle()
	end
end

function OnDestroy()
end
