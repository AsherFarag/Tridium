#include "tripch.h"
#include "GameObject.h"
#include "Components/Types.h"

namespace Tridium {

    GameObject::GameObject( EntityID a_ID )
        : m_ID( a_ID )
    {
    }

    void GameObject::Init( const std::string& name )
    {
        AddComponent<TagComponent>( name );
        AddComponent<TransformComponent>();
    }

    std::string& GameObject::GetTag() const
    {
        return GetComponent<TagComponent>().Tag;
    }

}