#pragma once
#include <Tridium/Application/Layer.h>

#if CONFIG_RUNTIME

namespace Tridium {

	//=================================================================================================
	// Runtime: The runtime layer is used for executable game builds.
	// It handles updating the scene and game logic.
	//=================================================================================================
	class Runtime final : public IAppLayer
	{
	public:

		//=============================================================================================
		Runtime() : IAppLayer( "Runtime" ) {}
		~Runtime() override = default;

		//=============================================================================================
		static Runtime* Get() { return s_Instance; }

	private:

		//=============================================================================================
		void OnAttach() override;
		void OnDetach() override;
		void OnEvent( Event& a_Event ) override;

		//=============================================================================================
		void OnUpdate();

	private:

		//=============================================================================================
		static Runtime* s_Instance;

	};

} // namespace Tridium

#endif // CONFIG_RUNTIME