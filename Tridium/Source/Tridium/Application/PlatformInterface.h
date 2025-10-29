#pragma once
#include <Tridium/Application/PlatformWindow.h>

namespace Tridium {

	//===========================================================================
	// PlatformType: The type of platform this interface is for.
	//===========================================================================
	enum class EPlatformType : uint8_t
	{
		Unknown = 0,

		// Desktop platforms
		Windows,
		Linux,
		MacOS,

		// Mobile platforms
		Android,
		iOS,

		// Console platforms
		PlayStation,
		Xbox,
	};

	//===========================================================================
	// Settings for the platform interface.
	//==========================================================================
	struct PlatformSettings
	{
	};

	//==========================================================================
	// Interface for platform-specific functionality.
	// Avoid using this class directly; instead, use the 'PlatformModule'.
	//==========================================================================
	class IPlatformInterface
	{
	public:

		virtual ~IPlatformInterface() = default;

		//=======================================================================
		virtual EPlatformType GetPlatformType() const = 0;

		//=======================================================================
		virtual UniquePtr< IPlatformWindow > CreateWindow( const PlatformWindowProps& a_Properties, IPlatformDisplay* a_Display = nullptr ) = 0;

		//=======================================================================
		virtual IPlatformDisplay* GetPrimaryDisplay() = 0;

		//=======================================================================
		virtual void PollEvents() = 0;

	private:

		//=======================================================================
		friend class Application;

		//=======================================================================
		virtual bool Setup( const PlatformSettings& a_PlatformSettings ) = 0;
		virtual void Teardown() = 0;

		//=======================================================================
		static UniquePtr< IPlatformInterface > Create( const PlatformSettings& a_PlatformSettings );
	};

} // namespace Tridium