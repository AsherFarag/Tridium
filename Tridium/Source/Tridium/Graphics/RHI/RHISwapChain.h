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

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHISwapChainDescriptor, RHISwapChain )
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t BufferCount = 0;
		ERHIScaleMode ScaleMode = ERHIScaleMode::None;
		ERHIFormat Format = ERHIFormat::RGBA8_UNORM;
		RHISampleSettings SampleSettings{};
		EnumFlags<ERHISwapChainFlags> Flags = ERHISwapChainFlags::Default;
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