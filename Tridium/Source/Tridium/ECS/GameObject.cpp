#include "tripch.h"
#include "GameObject.h"
#include "Components/Types.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

    std::vector<std::pair<Refl::MetaType, Component*>> GameObject::GetAllComponents() const
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

    const std::vector<GameObject>& GameObject::GetChildren() const {
		return GetTransform().GetChildren();
    }

}