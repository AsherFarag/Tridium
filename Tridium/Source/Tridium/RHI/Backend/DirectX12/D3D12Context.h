#pragma once
#include "D3D12Common.h"
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	//=======================================================
	// D3D12 Context
	//  A context for global D3D12 data.
	class D3D12Context : public ISingleton<D3D12Context, /* _ExplicitSetup */ false>
	{
	public:

		// A global storage for Wide Strings
		// Work around for owning strings as Tridium uses Strings instead of Wide Strings
		Array<WString> StringStorage;
	};
	//=======================================================
}