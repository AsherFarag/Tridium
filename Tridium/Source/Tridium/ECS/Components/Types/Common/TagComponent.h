#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	DEFINE_COMPONENT( TagComponent )
	{
	public:
		TagComponent()
			: Tag("Default") {}
		TagComponent( const TagComponent& ) = default;
		TagComponent( const std::string & a_Tag )
			: Tag( a_Tag ) {}

		operator std::string& ( ) { return Tag; }
		operator const std::string& ( ) const { return Tag; }
		bool operator ==( const TagComponent & other ) const { return other.Tag == Tag; }

	public:
		std::string Tag;
	};

}