local Time = 0.0
local SpawnTimer = 0.0
local SpawnRate = 2.0

local function SpawnObstacle()
	local go = GameObject.new()
	go:AddMesh()
	go:AddScript("Content/Scripts/Game/Obstacle.lua")
	go:AddSphereCollider()

	go:GetTransform().Position = gameObject:GetTransform().Position
end

function OnConstruct()
end

function OnUpdate( deltaTime )

	gameObject:GetTransform().Position.x = math.sin(Time)

	Time = Time + deltaTime
	SpawnTimer = SpawnTimer + deltaTime
	if SpawnTimer >= SpawnRate then
		SpawnTimer = SpawnTimer - SpawnRate
		SpawnObstacle()
	end
	
end

function OnDestroy()
end
