#pragma once
#include "FilePath.h"

namespace Tridium {

	class ISerializable
	{
	public:
		bool Serialize( const IO::FilePath& a_Path )
		{
        #if 1
			return SerializeText( a_Path );
        #else
			return SerializeBinary( a_Path );
        #endif // 1
		}

		bool Deserialize( const IO::FilePath& a_Path )
		{
        #if 1
			return DeserializeText( a_Path );
        #else
			return DeserializeBinary( a_Path );
        #endif // 1
		}

	protected:
		virtual bool SerializeText( const IO::FilePath& a_Path ) { return false; }
		virtual bool SerializeBinary( const IO::FilePath& a_Path ) { return false; }
		 
		virtual bool DeserializeText( const IO::FilePath& a_Path ) { return false; }
		virtual bool DeserializeBinary( const IO::FilePath& a_Path ) { return false; }
	};

}