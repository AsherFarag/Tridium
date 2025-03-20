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

	DEFINE_RHI_RESOURCE( SwapChain,
		virtual bool Present() = 0;
		virtual RHITextureRef GetBackBuffer() = 0;
		virtual bool Resize( uint32_t a_Width, uint32_t a_Height ) = 0;
		)
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t BufferCount = 0;
		ERHIUsageHint BufferUsage = ERHIUsageHint::RenderTarget;
		ERHIScaleMode ScaleMode = ERHIScaleMode::None;
		ERHITextureFormat Format = ERHITextureFormat::RGBA8;
		RHISampleSettings SampleSettings{};
		EnumFlags<ERHISwapChainFlags> Flags = ERHISwapChainFlags::Default;
	};

}