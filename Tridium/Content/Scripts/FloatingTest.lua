local time = 0.0

function OnConstruct()
end

function OnUpdate( deltaTime )
	time = time + deltaTime
	gameObject:GetTransform().Position.y = math.sin(time) * 1.0
	gameObject:GetTransform().Scale.y = math.sin(time) * 1.0
end

function OnDestroy()
end
