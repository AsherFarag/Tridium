#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	DEFINE_COMPONENT( TagComponent )
	{
		REFLECT;
	public:
		TagComponent()
			: Tag("Tag") {}
		TagComponent( const TagComponent& ) = default;
		TagComponent( const std::string & a_Tag )
			: Tag( a_Tag ) {}
		TagComponent( std::string&& a_Tag )
			: Tag( std::move(a_Tag) ) {}

		operator std::string& ( ) { return Tag; }
		operator const std::string& ( ) const { return Tag; }
		bool operator ==( const TagComponent & other ) const { return other.Tag == Tag; }

		static inline const uint32_t MaxSize() { return 256u; }

	public:
		std::string Tag;
	};

}