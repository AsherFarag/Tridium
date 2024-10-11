#include "tripch.h"
#include "TagComponent.h"

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( TagComponent )
		BASE( Component )
		PROPERTY( Tag, FLAGS( Serialize, EditAnywhere ) )
		FUNCTION( MaxSize )
	END_REFLECT( TagComponent )

	TagComponent::TagComponent() 
		: Tag() {}
	TagComponent::TagComponent( const std::string& a_Tag ) 
		: Tag( a_Tag ) {}
	TagComponent::TagComponent( std::string&& a_Tag ) 
		: Tag( a_Tag ) {}
}
