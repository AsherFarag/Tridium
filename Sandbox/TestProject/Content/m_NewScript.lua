function OnBeginPlay()
    print( "Begun Play!" )
    CharacterControllerComponent.AddToGameObject(gameObject)
end

function OnUpdate()
end

function OnDestroy()
    print("Destroyed!")
end