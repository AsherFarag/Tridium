#include "tripch.h"
#include "TestComponent.h"

template<>
struct ::Tridium::Refl::Reflector<::Tridium::TestComponent>
{
	using ClassType = TestComponent;
	Reflector() {
		using enum ::Tridium::Refl::EPropertyFlag;
		using namespace entt::literals;
		auto meta = entt::meta<TestComponent>();
		meta.type( "TestComponent"_hs );
		meta.prop( ::Tridium::Refl::Internal::CleanClassNamePropID, "TestComponent" );
		meta.prop( ::Tridium::Refl::Internal::YAMLSerializeFuncID,
			+[]( ::Tridium::IO::Archive& a_Archive, const ::Tridium::Refl::MetaAny& a_Data ) { ::Tridium::Refl::Internal::SerializeClass<TestComponent>( a_Archive, a_Data ); } );
		meta.prop( ::Tridium::Editor::Internal::DrawPropFuncID,
			+[]( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::PropertyFlags a_Flags ) -> bool
			{ return ::Tridium::Refl::Internal::DrawClassAsProperty<TestComponent>( a_Name, a_Handle, a_Flags ); } );
		meta.prop( ::Tridium::Refl::Internal::IsComponentPropID );
		meta.prop( ::Tridium::Refl::Internal::AddToGameObjectPropID,
			+[]( GameObject a_GameObject ) { a_GameObject.TryAddComponent<TestComponent>(); } );
		meta.base<Component>();
	}
};

static volatile ::Tridium::Refl::Reflector<::Tridium::TestComponent> s_ReflectorTestComponent;
