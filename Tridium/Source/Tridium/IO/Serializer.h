#pragma once
#include "FilePath.h"

namespace Tridium::IO {

	class Serializer
	{
	public:
		static void Serialize( const FilePath& a_Path, const void* a_Data, size_t a_Size );
	};

}