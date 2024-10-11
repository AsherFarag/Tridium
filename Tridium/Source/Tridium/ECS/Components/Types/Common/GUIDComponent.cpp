#include "tripch.h"
#include "GUIDComponent.h"

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( GUIDComponent )
		BASE( Component )
		PROPERTY( m_ID, FLAGS( Serialize, VisibleAnywhere ) )
	END_REFLECT( GUIDComponent )

	GUIDComponent::GUIDComponent( GUID a_ID )
		: m_ID( a_ID ) {}

}
