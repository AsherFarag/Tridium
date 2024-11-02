#include "tripch.h"
#include "GameObject.h"
#include "Components/Types.h"

#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {

    BEGIN_REFLECT( GameObject )
		FUNCTION( GetGUID )
		FUNCTION( GetTag )
		FUNCTION( GetTransform )
		FUNCTION( GetWorldTransform )
		FUNCTION( GetLocalTransform )
		FUNCTION( HasParent )
		FUNCTION( GetParent )
		FUNCTION( AttachToParent )
		FUNCTION( DetachFromParent )
		FUNCTION( AttachChild )
		FUNCTION( DetachChild )
		FUNCTION( GetChild )
		FUNCTION( GetChildren )
        PROPERTY( m_ID )
    END_REFLECT( GameObject )

    GameObject::GameObject( EntityID a_ID )
        : m_ID( a_ID )
    {
    }

    std::vector<std::pair<Refl::MetaType, Component*>> Tridium::GameObject::GetAllComponents() const
    {
        std::vector<std::pair<Refl::MetaType, Component*>> components;
		// Reserve a magic number of components
		components.reserve( 16 );

		for ( auto&& [id, componentStorage] : Application::GetScene()->m_Registry.storage() )
		{
            if ( componentStorage.contains( m_ID ) )
                components.emplace_back( 
                    entt::resolve( componentStorage.type() ),
                    static_cast<Component*>( componentStorage.value( m_ID ) ) 
                );
		}

        return std::move(components);
    }

    GUID GameObject::GetGUID() const {
        return GetComponent<GUIDComponent>().GetID();
    }

    std::string& GameObject::GetTag() const {
        return GetComponent<TagComponent>().Tag;
    }

    TransformComponent& GameObject::GetTransform() const {
        return GetComponent<TransformComponent>();
    }

    Matrix4 GameObject::GetWorldTransform() const {
        return GetTransform().GetWorldTransform();
    }

    Matrix4 GameObject::GetLocalTransform() const {
        return GetTransform().GetLocalTransform();
    }

    bool GameObject::HasParent() const {
        return GetParent().IsValid();
    }

    GameObject GameObject::GetParent() const {
        return GetTransform().GetParent();
    }

    void GameObject::AttachToParent( GameObject a_Parent ) {
        GetTransform().AttachToParent( a_Parent );
    }

    void GameObject::DetachFromParent() {
        GetTransform().DetachFromParent();
    }

    void GameObject::AttachChild( GameObject a_Child ) {
        GetTransform().AttachChild( a_Child );
    }

    void GameObject::DetachChild( GameObject a_Child ) {
        GetTransform().DetachChild( a_Child );
    }

    GameObject GameObject::GetChild( const std::string& a_Tag ) const {
        return GetTransform().GetChild( a_Tag );
    }

    std::vector<GameObject>& GameObject::GetChildren() {
        return GetTransform().GetChildren();
    }

}