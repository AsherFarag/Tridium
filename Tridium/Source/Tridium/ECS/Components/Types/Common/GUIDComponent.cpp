#include "tripch.h"
#include "GUIDComponent.h"

namespace Tridium {

	template<> struct ::Tridium::Refl::Reflector<GUIDComponent> {
		using ClassType = GUIDComponent;
		Reflector() 
		{
			using enum ::Tridium::Refl::EPropertyFlag; 
			using namespace entt::literals; 
			auto meta = entt::meta<GUIDComponent>();
			meta.type( "GUIDComponent"_hs ); 
			meta.prop( ::Tridium::Refl::Internal::CleanClassNamePropID, "GUIDComponent" );
			meta.prop( ::Tridium::Refl::Internal::YAMLSerializeFuncID, 
				+[]( ::Tridium::IO::Archive& a_Archive, const ::Tridium::Refl::MetaAny& a_Data )
				{ ::Tridium::Refl::Internal::SerializeClass<GUIDComponent>( a_Archive, a_Data ); } );
			meta.prop( ::Tridium::Editor::Internal::DrawPropFuncID, 
				+[]( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::PropertyFlags a_Flags ) -> bool 
				{ return ::Tridium::Refl::Internal::DrawClassAsProperty<GUIDComponent>( a_Name, a_Handle, a_Flags ); } );
			meta.prop( ::Tridium::Refl::Internal::IsComponentPropID );
			meta.prop( ::Tridium::Refl::Internal::AddToGameObjectPropID, 
				+[]( GameObject a_GameObject ) { a_GameObject.TryAddComponent<GUIDComponent>(); } );
		}
	}; ::Tridium::Refl::Reflector<GUIDComponent> GUIDComponent::___StaticInitializer_GUIDComponent;

}
