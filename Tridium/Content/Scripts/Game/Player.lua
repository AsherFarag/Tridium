local Speed = 2.0
local Range = 1.0
MaxHealth = 100.0
CurrentHealth = 100.0

local function Move( deltaTime, direction )
	gameObject:GetTransform().Position.x = deltaTime * direction * Speed +  gameObject:GetTransform().Position.x
end

local function OnDeath()
	OnGameOver()
end

function OnConstruct()
end

function OnUpdate( deltaTime )

	if CurrentHealth > 0 then

	-- A Left
	if Input.IsKeyDown(65) then
		Move(deltaTime, -1)
	end
	-- D Right
	if Input.IsKeyDown(68) then
		Move(deltaTime, 1)
	end

	if gameObject:GetTransform().Position.x < -Range then
		gameObject:GetTransform().Position.x = -Range
	end

	if gameObject:GetTransform().Position.x > Range then
		gameObject:GetTransform().Position.x = Range
	end

	if gameObject:GetSphereCollider().IsColliding then
		CurrentHealth = CurrentHealth - deltaTime * 25
	end

	if CurrentHealth < 0.0 then
		OnDeath()
	end

	end
end

function OnDestroy()
end