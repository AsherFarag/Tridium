#include "tripch.h"
#include "TagComponent.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( OldTagComponent, Scriptable )
		BASE( Component )
		PROPERTY( Tag, Serialize | EditAnywhere )
		FUNCTION( MaxSize )
	END_REFLECT( OldTagComponent )

	OldTagComponent::OldTagComponent() 
		: Tag() {}
	OldTagComponent::OldTagComponent( const std::string& a_Tag ) 
		: Tag( a_Tag ) {}
	OldTagComponent::OldTagComponent( std::string&& a_Tag ) 
		: Tag( a_Tag ) {}
}
