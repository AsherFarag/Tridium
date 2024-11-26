#pragma once
#if IS_EDITOR

#include "Panel.h"

namespace Tridium::Editor {

	class AssetRegistryPanel : public Panel
	{
	public:
		AssetRegistryPanel()
			: Panel( "Asset Registry" ) {}

		virtual void OnImGuiDraw() override;
	};

}

#endif // IS_EDITOR