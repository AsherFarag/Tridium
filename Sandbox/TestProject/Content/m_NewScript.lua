function OnBeginPlay()
    print( "Begun Play!" )
end

function OnUpdate()
    newPos = transform.worldPosition
    newPos.z = newPos.z + 0.01
    transform.worldPosition = newPos
end

function OnDestroy()
    print("Destroyed!")
end