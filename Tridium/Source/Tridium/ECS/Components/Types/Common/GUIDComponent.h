#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	DEFINE_COMPONENT( GUIDComponent )
	{
		REFLECT;
	public:
		GUIDComponent() : m_ID( GUID::Create() ) {}
		GUIDComponent( GUID id) : m_ID(id) {}

		void SetID(GUID id) { m_ID = id; }
		GUID GetID() const { return m_ID; }

	private:
		GUID m_ID;
	};

}