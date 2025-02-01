#pragma once
#include <Tridium/Core/Types.h>

#if CONFIG_DEBUG
	#define RHI_DEBUG_ENABLED 1
#else
	#define RHI_DEBUG_ENABLED 0
#endif // CONFIG_DEBUG

namespace Tridium {

	//====================================
	// Rendering Hardware Interface Type
	//====================================
	enum class ERHInterfaceType : uint8_t
	{
		Null,
		OpenGL,
		DirectX11,
		DirectX12,
		Vulkan,
		Metal,
	};

	//===========================
	// RHI Feature Support
	//===========================
	enum class ERHIFeatureSupport : uint8_t
	{
		// The RHI feature is completely unavailable at runtime
		Unsupported,

		// The RHI feature can be available at runtime based on hardware or driver
		RuntimeDependent,

		// The RHI feature is guaranteed to be available at runtime.
		RuntimeGuaranteed,
	};

	//===========================
	// RHI Configuration
	//===========================
	struct RHIConfig
	{
		ERHInterfaceType RHIType = ERHInterfaceType::Null;
		bool UseDebug = false;
	};

	//===========================
	// Shading Path
	//===========================
	enum class ERHIShadingPath : uint8_t
	{
		Deferred,
		Forward,
		COUNT,
	};

	//===========================
	// Comparison Functions
	//===========================
	enum class ECompareFunction : uint8_t
	{
		Never,         // False
		Less,          // <
		Equal,         // ==
		LessEqual,	   // <=
		Greater,	   // >
		NotEqual,	   // !=
		GreaterEqual,  // >=
		Always,		   // True
		COUNT,
	};

	//===========================
	// RHI Data Type
	//===========================
	struct RHIDataType
	{
		enum EComponentType : uint8_t
		{
			Float16 = 0u,
			Float32,
			Float64,
			UNorm8,  // Unsigned Normalised 8-bit, 0-1
			SNorm8,  // Signed Normalised 8-bit, -1 to 1
			UNorm16, // Unsigned Normalised 16-bit, 0-1
			SNorm16, // Signed Normalised 16-bit, -1 to 1
			UInt8,
			SInt8,
			UInt16,
			SInt16,
			UInt32,
			SInt32,
			UInt64,
			SInt64,
			COUNT,
			Unknown
		};

		EComponentType Type = EComponentType::Unknown;
		uint8_t Components = 1u;

		constexpr RHIDataType() = default;
		constexpr RHIDataType( EComponentType a_Type, uint8_t a_Components = 1u )
			: Type( a_Type ), Components( a_Components )
		{
		}

		constexpr size_t Size()
		{
			switch ( Type )
			{
				using enum RHIDataType::EComponentType;
				case Float16: return 2u * Components;
				case Float32: return 4u * Components;
				case Float64: return 8u * Components;
				case UNorm8: return 1u * Components;
				case SNorm8: return 1u * Components;
				case UNorm16: return 2u * Components;
				case SNorm16: return 2u * Components;
				case UInt8: return 1u * Components;
				case SInt8: return 1u * Components;
				case UInt16: return 2u * Components;
				case SInt16: return 2u * Components;
				case UInt32: return 4u * Components;
				case SInt32: return 4u * Components;
				case UInt64: return 8u * Components;
				case SInt64: return 8u * Components;
				default: return 0u;
			}
		}
	};

	//========================================
	// RHI Helper Functions
	//========================================
	namespace RHI {
		template<typename T>
		constexpr RHIDataType GetDataType();
	}

	//===========================
	// Common RHI Data Types
	//===========================
	namespace RHIDataTypes {
		static constexpr RHIDataType Vector2{ RHIDataType::Float32, 2u };
		static constexpr RHIDataType Vector3{ RHIDataType::Float32, 3u };
		static constexpr RHIDataType Vector4{ RHIDataType::Float32, 4u };
		static constexpr RHIDataType Matrix2{ RHIDataType::Float32, 4u };
		static constexpr RHIDataType Matrix3{ RHIDataType::Float32, 9u };
		static constexpr RHIDataType Matrix4{ RHIDataType::Float32, 16u };
		static constexpr RHIDataType iVector2{ RHIDataType::SInt32, 2u };
		static constexpr RHIDataType iVector3{ RHIDataType::SInt32, 3u };
		static constexpr RHIDataType iVector4{ RHIDataType::SInt32, 4u };
	}

	template<> constexpr RHIDataType RHI::GetDataType<float32_t>() { return RHIDataType{ RHIDataType::Float32, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<float64_t>() { return RHIDataType{ RHIDataType::Float64, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<uint8_t>() { return RHIDataType{ RHIDataType::UNorm8, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<int8_t>() { return RHIDataType{ RHIDataType::SNorm8, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<uint16_t>() { return RHIDataType{ RHIDataType::UNorm16, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<int16_t>() { return RHIDataType{ RHIDataType::SNorm16, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<uint32_t>() { return RHIDataType{ RHIDataType::UInt32, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<int32_t>() { return RHIDataType{ RHIDataType::SInt32, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<uint64_t>() { return RHIDataType{ RHIDataType::UInt64, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<int64_t>() { return RHIDataType{ RHIDataType::SInt64, 1u }; }
	template<> constexpr RHIDataType RHI::GetDataType<Vector2>() { return RHIDataTypes::Vector2; }
	template<> constexpr RHIDataType RHI::GetDataType<Vector3>() { return RHIDataTypes::Vector3; }
	template<> constexpr RHIDataType RHI::GetDataType<Vector4>() { return RHIDataTypes::Vector4; }
	template<> constexpr RHIDataType RHI::GetDataType<Matrix2>() { return RHIDataTypes::Matrix2; }
	template<> constexpr RHIDataType RHI::GetDataType<Matrix3>() { return RHIDataTypes::Matrix3; }
	template<> constexpr RHIDataType RHI::GetDataType<Matrix4>() { return RHIDataTypes::Matrix4; }
	template<> constexpr RHIDataType RHI::GetDataType<iVector2>() { return RHIDataTypes::iVector2; }
	template<> constexpr RHIDataType RHI::GetDataType<iVector3>() { return RHIDataTypes::iVector3; }
	template<> constexpr RHIDataType RHI::GetDataType<iVector4>() { return RHIDataTypes::iVector4; }

} // namespace Tridium