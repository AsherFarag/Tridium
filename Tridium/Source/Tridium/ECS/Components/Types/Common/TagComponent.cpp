#include "tripch.h"
#include "TagComponent.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( TagComponent, Scriptable )
		BASE( Component )
		PROPERTY( Tag, Serialize | EditAnywhere )
		FUNCTION( MaxSize )
	END_REFLECT( TagComponent )

	TagComponent::TagComponent() 
		: Tag() {}
	TagComponent::TagComponent( const std::string& a_Tag ) 
		: Tag( a_Tag ) {}
	TagComponent::TagComponent( std::string&& a_Tag ) 
		: Tag( a_Tag ) {}
}
