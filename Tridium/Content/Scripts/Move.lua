function OnConstruct()
end

function OnUpdate( deltaTime )
	gameObject:GetTransform().Position.x = gameObject:GetTransform().Position.x + deltaTime * -2
end

function OnDestroy()
end
