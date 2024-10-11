#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class GUIDComponent : public Component
	{
		REFLECT( GUIDComponent );
	public:
		GUIDComponent() : m_ID( GUID::Create() ) {}
		GUIDComponent( GUID a_ID );

		void SetID(GUID id) { m_ID = id; }
		GUID GetID() const { return m_ID; }

	private:
		GUID m_ID;
	};

}