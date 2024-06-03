local Speed = 2.0

local function Move( deltaTime, direction )
	gameObject:GetTransform().Position.x = deltaTime * direction * Speed +  gameObject:GetTransform().Position.x
end

function OnConstruct()
end

function OnUpdate( deltaTime )
	-- A Left
	if Input.IsKeyDown(65) then
		Move(deltaTime, -1)
	end
	-- D Right
	if Input.IsKeyDown(68) then
		Move(deltaTime, 1)
	end

end

function OnDestroy()
end
