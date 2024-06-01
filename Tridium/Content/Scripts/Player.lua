MaxHealth = 100.0
CurrentHealth = 100.0	

local function OnDeath()
	gameObject:Destroy()
end

function OnConstruct()
end

function OnUpdate( deltaTime )
	Pos = gameObject:GetTransform().Position
	if ( Pos.x > -0.5 and Pos.x < 0.5 and
	    Pos.y > -0.5 and Pos.y < 0.5  and
	    Pos.z > -0.5 and Pos.z < 0.5 )  then
		CurrentHealth = CurrentHealth - deltaTime * 10
	end

	if CurrentHealth < 0.0 then
		OnDeath()
	end
end

function OnDestroy()
	print("Game Over!")
	Quit()
end
