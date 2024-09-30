#include "tripch.h"
#include "TagComponent.h"

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( TagComponent )
		BASE( Component )
		PROPERTY( Tag, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( TagComponent )

}
