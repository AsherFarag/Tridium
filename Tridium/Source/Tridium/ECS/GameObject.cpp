#include "tripch.h"
#include "GameObject.h"
#include "Components/Types.h"

namespace Tridium {

    GameObject::GameObject( EntityID a_ID, const std::string& name )
        : m_ID( a_ID )
    {
        AddComponent<TagComponent>( name );
        AddComponent<TransformComponent>();
    }

}