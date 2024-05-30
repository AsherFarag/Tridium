local health = 10

function OnConstruct()
end

function OnUpdate( deltaTime )
	if gameObject:GetTransform().Position.y < 0 then
		health = health - deltaTime * 3
	end
	
	print(health)

	if health < 0 then
		gameObject:Destroy()
	end 
end

function OnDestroy()
end
