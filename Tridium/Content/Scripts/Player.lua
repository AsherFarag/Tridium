MaxHealth = 100.0
CurrentHealth = 100.0	

local function OnDeath()
	gameObject:Destroy()
end

function OnConstruct()
end

function OnUpdate( deltaTime )
	Pos = gameObject:GetTransform().Position
	if (  gameObject:GetSphereCollider.IsColliding  )  then
		CurrentHealth = CurrentHealth - deltaTime * 1
	end

	if CurrentHealth < 0.0 then
		OnDeath()
	end
end

function OnDestroy()
	print("Game Over!")
	App.Quit()
end
