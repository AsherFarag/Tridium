#pragma once
#include "RHIResource.h"
#include "RHITexture.h"

namespace Tridium {

	enum class ERHISwapChainFlags : uint8_t
	{
		None = 0,
		UseVSync = 1 << 0,

		Default = UseVSync
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ERHISwapChainFlags );

	enum class ERHIScaleMode : uint8_t
	{
		None,
		Stretch,
		AspectRatioStretch
	};

	struct RHISampleSettings
	{
		uint32_t Count = 1;
		uint32_t Quality = 0;
	};

	//=====================================================================
	// RHI Swap Chain
	//  A swap chain is a collection of buffers that are used to present the final image to the screen.
	//=====================================================================

	struct RHISwapChainDescriptor
	{
		using ResourceType = class RHISwapChain;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t BufferCount = 0;
		ERHIScaleMode ScaleMode = ERHIScaleMode::None;
		ERHIFormat Format = ERHIFormat::RGBA8_UNORM;
		RHISampleSettings SampleSettings{};
		EnumFlags<ERHISwapChainFlags> Flags = ERHISwapChainFlags::Default;
		String Name{};

		constexpr auto& SetWidth( uint32_t a_Width ) { Width = a_Width; return *this; }
		constexpr auto& SetHeight( uint32_t a_Height ) { Height = a_Height; return *this; }
		constexpr auto& SetBufferCount( uint32_t a_BufferCount ) { BufferCount = a_BufferCount; return *this; }
		constexpr auto& SetScaleMode( ERHIScaleMode a_ScaleMode ) { ScaleMode = a_ScaleMode; return *this; }
		constexpr auto& SetFormat( ERHIFormat a_Format ) { Format = a_Format; return *this; }
		constexpr auto& SetSampleSettings( const RHISampleSettings& a_SampleSettings ) { SampleSettings = a_SampleSettings; return *this; }
		constexpr auto& SetFlags( EnumFlags<ERHISwapChainFlags> a_Flags ) { Flags = a_Flags; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHISwapChain )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHISwapChain, ERHIResourceType::SwapChain );

		RHISwapChain( const DescriptorType& a_Desc )
			: m_Desc( a_Desc ) {}

		virtual bool Present() = 0;
		virtual RHITextureRef GetBackBuffer() = 0;
		virtual bool Resize( uint32_t a_Width, uint32_t a_Height ) = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
	};

}