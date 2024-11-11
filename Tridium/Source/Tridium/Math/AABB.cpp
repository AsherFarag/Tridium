#include "tripch.h"
#include "AABB.h"

namespace Tridium {

	const AABB AABB::s_MaxAABB = AABB( Vector3( -FLT_MAX ), Vector3( FLT_MAX ) );
	const Rect Rect::s_MaxRect = Rect( Vector2( -FLT_MAX ), Vector2( FLT_MAX ) );

} // namespace Tridium