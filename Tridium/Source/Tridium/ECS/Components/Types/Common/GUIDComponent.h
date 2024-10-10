#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class GUIDComponent : public Component
	{
		template<typename T> friend class ::Tridium::Refl::Reflector; 
		[[maybe_unused]] static ::Tridium::Refl::Reflector<GUIDComponent> ___StaticInitializer_GUIDComponent;
	public:
		GUIDComponent() : m_ID( GUID::Create() ) {}
		GUIDComponent( GUID a_ID) : m_ID(a_ID) {}

		void SetID(GUID id) { m_ID = id; }
		GUID GetID() const { return m_ID; }

	private:
		GUID m_ID;
	};

}