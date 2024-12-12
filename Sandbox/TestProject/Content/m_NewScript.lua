function OnBeginPlay()
    print( "Begun Play!" )
end

function OnUpdate()
    newPos = transform.worldPosition
    newPos.y = newPos.y + 1.1
    transform.worldPosition = newPos
end

function OnDestroy()
    print("Destroyed!")
end