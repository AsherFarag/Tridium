function OnConstruct()
end

function OnUpdate( deltaTime )
	gameObject:GetTransform().Position.x = (1.0 * deltaTime)  * gameObject:GetTransform().Position.y + gameObject:GetTransform().Position.x
end

function OnDestroy()
end