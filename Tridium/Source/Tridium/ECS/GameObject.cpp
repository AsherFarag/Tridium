#include "tripch.h"
#include "GameObject.h"
#include "Components/Types.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

    Array<Pair<Refl::MetaType, Component*>> OldGameObject::GetAllComponents() const
    {
        Array<Pair<Refl::MetaType, Component*>> components;
		// Reserve a magic number of components
		components.Reserve( 16 );

		for ( auto&& [id, componentStorage] : SceneManager::GetActiveScene()->GetECS().Storage() )
		{
            if ( componentStorage.contains( m_ID ) )
                components.EmplaceBack( 
                    Refl::ResolveMetaType( componentStorage.type() ),
                    Cast<Component*>( componentStorage.value( m_ID ) )
                );
		}

        return components;
    }

    GUID OldGameObject::GetGUID() const 
    {
        return GetComponent<GUIDComponent>().GetID();
    }

    TagComponent* OldGameObject::GetTagComponent() const
    {
        return TryGetComponent<TagComponent>();
    }

    const String& OldGameObject::GetTag() const
    {
		if ( auto* tagComponent = GetTagComponent() )
			return tagComponent->Tag;

		static const String s_Unknown{ "Unknown" };
		return s_Unknown;
    }

	//////////////////////////////////////////////////////////////////////////
	// GameObject Flags
	//////////////////////////////////////////////////////////////////////////

    EnumFlags<EGameObjectFlags> OldGameObject::GetFlags()
    {
		if ( auto* flagsComponent = TryGetComponent<GameObjectFlagsComponent>() )
			return flagsComponent->Flags;
		return AddComponent<GameObjectFlagsComponent>( EGameObjectFlags::Enabled ).Flags;
    }

    void OldGameObject::SetActive( bool a_Active, bool a_PropagateToChildren )
    {
		static const Refl::MetaType NativeScriptComponentType = Refl::ResolveMetaType<NativeScriptComponent>();

        EnumFlags<EGameObjectFlags>& flags = GetComponent<GameObjectFlagsComponent>().Flags;
		flags.SetFlag( EGameObjectFlags::Active, a_Active );

        if ( a_PropagateToChildren )
        {
            for ( auto child : GetChildren() )
            {
                child.SetActive( a_Active, a_PropagateToChildren );
            }
        }
    }

    void OldGameObject::SetVisible( bool a_Visible, bool a_PropagateToChildren )
    {
        static const Refl::MetaType NativeScriptComponentType = Refl::ResolveMetaType<NativeScriptComponent>();

        EnumFlags<EGameObjectFlags>& flags = GetComponent<GameObjectFlagsComponent>().Flags;
		flags.SetFlag( EGameObjectFlags::Visible, a_Visible );

		if ( a_PropagateToChildren )
		{
			for ( auto child : GetChildren() )
			{
				child.SetVisible( a_Visible, a_PropagateToChildren );
			}
		}
    }

    void OldGameObject::SetEnabled( bool a_Enabled, bool a_PropagateToChildren )
    {
        static const Refl::MetaType NativeScriptComponentType = Refl::ResolveMetaType<NativeScriptComponent>();

        EnumFlags<EGameObjectFlags>& flags = GetComponent<GameObjectFlagsComponent>().Flags;
        flags.SetFlag( EGameObjectFlags::Enabled, a_Enabled );

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

    TransformComponent& OldGameObject::GetTransform() const 
    {
        return GetComponent<TransformComponent>();
    }

    Matrix4 OldGameObject::GetWorldTransform() const 
    {
        return GetTransform().GetWorldTransform();
    }

    Matrix4 OldGameObject::GetLocalTransform() const 
    {
        return GetTransform().GetLocalTransform();
    }

    bool OldGameObject::HasParent() const 
    {
        return GetParent().IsValid();
    }

    OldGameObject OldGameObject::GetParent() const 
    {
        return GetTransform().GetParent();
    }

    void OldGameObject::AttachToParent( OldGameObject a_Parent ) 
    {
        GetTransform().AttachToParent( a_Parent );
    }

    void OldGameObject::DetachFromParent() 
    {
        GetTransform().DetachFromParent();
    }

    void OldGameObject::AttachChild( OldGameObject a_Child )
    {
        GetTransform().AttachChild( a_Child );
    }

    void OldGameObject::DetachChild( OldGameObject a_Child )
    {
        GetTransform().DetachChild( a_Child );
    }

    OldGameObject OldGameObject::GetChild( const std::string& a_Tag ) const
    {
        return GetTransform().GetChild( a_Tag );
    }

    std::vector<OldGameObject>& OldGameObject::GetChildren() 
    {
        return GetTransform().GetChildren();
    }

    const std::vector<OldGameObject>& OldGameObject::GetChildren() const
    {
		return GetTransform().GetChildren();
    }

}