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

		for ( auto&& [id, componentStorage] : SceneManager::GetActiveScene()->GetECS().Storage() )
		{
            if ( componentStorage.contains( m_ID ) )
                components.emplace_back( 
                    Refl::ResolveMetaType( componentStorage.type() ),
                    static_cast<Component*>( componentStorage.value( m_ID ) )
                );
		}

        return components;
    }

    GUID GameObject::GetGUID() const 
    {
        return GetComponent<GUIDComponent>().GetID();
    }

    TagComponent* GameObject::GetTagComponent() const
    {
        return TryGetComponent<TagComponent>();
    }

    const String& GameObject::GetTag() const
    {
		if ( auto* tagComponent = GetTagComponent() )
			return tagComponent->Tag;

		static const String s_Unknown{ "Unknown" };
		return s_Unknown;
    }

	//////////////////////////////////////////////////////////////////////////
	// GameObject Flags
	//////////////////////////////////////////////////////////////////////////

    GameObjectFlags GameObject::GetFlags()
    {
		if ( auto* flagsComponent = TryGetComponent<GameObjectFlagsComponent>() )
			return flagsComponent->Flags;
		return AddComponent<GameObjectFlagsComponent>( DefaultGameObjectFlags ).Flags;
    }

    void GameObject::SetActive( bool a_Active, bool a_PropagateToChildren )
    {
		static const Refl::MetaType NativeScriptComponentType = Refl::ResolveMetaType<NativeScriptComponent>();

		GameObjectFlags& flags = GetComponent<GameObjectFlagsComponent>().Flags;
		flags.SetFlag( EGameObjectFlag::Active, a_Active );

        if ( a_PropagateToChildren )
        {
            for ( auto child : GetChildren() )
            {
                child.SetActive( a_Active, a_PropagateToChildren );
            }
        }
    }

    void GameObject::SetVisible( bool a_Visible, bool a_PropagateToChildren )
    {
        static const Refl::MetaType NativeScriptComponentType = Refl::ResolveMetaType<NativeScriptComponent>();

		GameObjectFlags& flags = GetComponent<GameObjectFlagsComponent>().Flags;
		flags.SetFlag( EGameObjectFlag::Visible, a_Visible );

		if ( a_PropagateToChildren )
		{
			for ( auto child : GetChildren() )
			{
				child.SetVisible( a_Visible, a_PropagateToChildren );
			}
		}
    }

    void GameObject::SetEnabled( bool a_Enabled, bool a_PropagateToChildren )
    {
        static const Refl::MetaType NativeScriptComponentType = Refl::ResolveMetaType<NativeScriptComponent>();

        GameObjectFlags& flags = GetComponent<GameObjectFlagsComponent>().Flags;
        flags.SetFlag( EGameObjectFlag::Enabled, a_Enabled );

        if ( a_PropagateToChildren )
        {
            for ( auto child : GetChildren() )
            {
                child.SetEnabled( a_Enabled, a_PropagateToChildren );
            }
        }
    }

	//////////////////////////////////////////////////////////////////////////
	// Transform Functions
	//////////////////////////////////////////////////////////////////////////

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