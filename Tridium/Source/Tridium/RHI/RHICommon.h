#pragma once
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Containers/Containers.h>

#if CONFIG_DEBUG
	#define RHI_DEBUG_ENABLED 1
#else
	#define RHI_DEBUG_ENABLED 0
#endif // CONFIG_DEBUG


namespace Tridium {

	enum class EGraphicsAPI : uint8_t
	{
		None = 0,
		OpenGL = 1,
		DirectX11 = 2,
		DirectX12 = 3,
		Vulkan = 4
	};

	struct RHIConfig
	{
		EGraphicsAPI GraphicsAPI = EGraphicsAPI::None;
		bool UseDebug = false;
	};

	// Core RHI functions
	namespace RHI {
		bool Initialise( const RHIConfig& a_Config );
		bool Shutdown();
		bool Present();
		EGraphicsAPI GetGraphicsAPI();
		const char* GetGraphicsAPIName( EGraphicsAPI a_API = EGraphicsAPI::None );
	}

	// RHI Query functions to get information about the current RHI
	namespace RHIQuery {
		constexpr bool IsGraphicsAPISupported( EGraphicsAPI a_API );
		constexpr bool IsMultithreadingSupported( EGraphicsAPI a_API = EGraphicsAPI::None );

		const RHIConfig& GetConfig();
		static constexpr uint32_t MaxTextureBindings = 32u;
		static constexpr uint32_t MaxColourTargets = 8u;
		static constexpr uint32_t MaxVertexAttributes = 16u;
		static constexpr uint32_t MaxShaderInputs = 32u;
	}

	//===========================
	// RHI Data Type
	//===========================
	enum class ERHIDataType : uint8_t
	{
		Float64 = 0u,
		Float32,
		Float16,
		UNorm8,
		SNorm8,
		UNorm16,
		SNorm16,
		UInt8,
		SInt8,
		UInt16,
		SInt16,
		UInt32,
		SInt32,
		COUNT,
		Unknown
	};

	// RHI Helper Functions
	namespace RHI {
		constexpr uint8_t GetDataTypeSize( ERHIDataType a_Type );

		template<typename T>
		constexpr ERHIDataType GetDataType();
	}

	//===========================

	template<> constexpr ERHIDataType RHI::GetDataType<float32_t>() { return ERHIDataType::Float32; }
	template<> constexpr ERHIDataType RHI::GetDataType<float64_t>() { return ERHIDataType::Float64; }
	template<> constexpr ERHIDataType RHI::GetDataType<uint8_t>()   { return ERHIDataType::UInt8; }
	template<> constexpr ERHIDataType RHI::GetDataType<int8_t>()    { return ERHIDataType::SInt8; }
	template<> constexpr ERHIDataType RHI::GetDataType<uint16_t>()  { return ERHIDataType::UInt16; }
	template<> constexpr ERHIDataType RHI::GetDataType<int16_t>()   { return ERHIDataType::SInt16; }
	template<> constexpr ERHIDataType RHI::GetDataType<uint32_t>()  { return ERHIDataType::UInt32; }
	template<> constexpr ERHIDataType RHI::GetDataType<int32_t>()   { return ERHIDataType::SInt32; }


} // namespace Tridium