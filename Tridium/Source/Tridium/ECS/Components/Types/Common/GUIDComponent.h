#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class OldGUIDComponent : public Component
	{
		REFLECT( OldGUIDComponent );
	public:
		OldGUIDComponent() : m_ID( GUID::Create() ) {}
		OldGUIDComponent( GUID a_ID );

		void SetID(GUID id) { m_ID = id; }
		GUID GetID() const { return m_ID; }

	private:
		GUID m_ID;
	};

}