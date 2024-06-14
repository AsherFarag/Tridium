local DeadZone = 10

function OnConstruct()
end

function OnUpdate( deltaTime )
	gameObject:GetTransform().Position.z = 
		gameObject:GetTransform().Position.z 
		+ deltaTime * 1;

	if gameObject:GetTransform().Position.z >= DeadZone then
		gameObject:Destroy()
	end
end

function OnDestroy()
end
