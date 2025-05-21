#pragma once
#include "RHIDefinitions.h"
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/Cast.h>
#include <Tridium/Containers/Span.h>
#include <Tridium/Containers/Tuple.h>
#include <Tridium/Containers/String.h>
#include <sstream> // For stringstream

namespace Tridium {

	namespace Detail {

		template<typename _Enum>
		inline constexpr bool AssignEnumValue( _Enum a_EnumVal, _Enum& o_Value )
		{
			o_Value = a_EnumVal;
			return true;
		}

		template<typename _Enum, size_t _Extent = std::dynamic_extent>
		static String FlagsToString(
			_Enum a_Flags,
			Span<const Pair<_Enum, StringView>, _Extent> a_FlagNames )
		{
			std::ostringstream oss;
			bool first = true;

			const EnumFlags<_Enum> flags( a_Flags );

			for ( const auto& [flag, name] : a_FlagNames )
			{
				if ( flags.HasFlag( flag ) )
				{
					if ( !first )
						oss << '|';
					oss << name;
					first = false;
				}
			}

			if ( first )
				return {};
			return oss.str();
		}

	}

	//=========================================================
	// ERHInterfaceType
	//=========================================================

	static constexpr StringView ToString( ERHInterfaceType a_API )
	{
		switch ( a_API )
		{
		case ERHInterfaceType::Null:       return "Null";
		case ERHInterfaceType::OpenGL:     return "OpenGL";
		case ERHInterfaceType::DirectX11:  return "DirectX11";
		case ERHInterfaceType::DirectX12:  return "DirectX12";
		case ERHInterfaceType::Vulkan:     return "Vulkan";
		case ERHInterfaceType::Metal:      return "Metal";
		default:                           return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_String, ERHInterfaceType& o_Value )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( a_String.data() ), a_String.size() ) )
		{
		case "Null"_H:       return Detail::AssignEnumValue( ERHInterfaceType::Null, o_Value );
		case "OpenGL"_H:     return Detail::AssignEnumValue( ERHInterfaceType::OpenGL, o_Value );
		case "DirectX11"_H:  return Detail::AssignEnumValue( ERHInterfaceType::DirectX11, o_Value );
		case "DirectX12"_H:  return Detail::AssignEnumValue( ERHInterfaceType::DirectX12, o_Value );
		case "Vulkan"_H:     return Detail::AssignEnumValue( ERHInterfaceType::Vulkan, o_Value );
		case "Metal"_H:      return Detail::AssignEnumValue( ERHInterfaceType::Metal, o_Value );
		}
		return false;
	}

	//=========================================================
	// EGPUVendorID
	//=========================================================
	
	static constexpr StringView ToString( EGPUVendorID a_Vendor )
	{
		switch ( a_Vendor )
		{
		case EGPUVendorID::Unknown:     return "Unknown";
		case EGPUVendorID::Invalid:     return "Invalid";
		case EGPUVendorID::Amd:         return "Amd";
		case EGPUVendorID::ImgTec:      return "ImgTec";
		case EGPUVendorID::Nvidia:      return "Nvidia";
		case EGPUVendorID::Arm:         return "Arm";
		case EGPUVendorID::Broadcom:    return "Broadcom";
		case EGPUVendorID::Qualcomm:    return "Qualcomm";
		case EGPUVendorID::Intel:       return "Intel";
		case EGPUVendorID::Apple:       return "Apple";
		case EGPUVendorID::Vivante:     return "Vivante";
		case EGPUVendorID::VeriSilicon: return "VeriSilicon";
		case EGPUVendorID::SamsungAMD:  return "SamsungAMD";
		case EGPUVendorID::Microsoft:   return "Microsoft";
		default:                   return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_String, EGPUVendorID& o_Value )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( a_String.data() ), a_String.size() ) )
		{
		case "Unknown"_H:     return Detail::AssignEnumValue( EGPUVendorID::Unknown, o_Value );
		case "Invalid"_H:     return Detail::AssignEnumValue( EGPUVendorID::Invalid, o_Value );
		case "Amd"_H:         return Detail::AssignEnumValue( EGPUVendorID::Amd, o_Value );
		case "ImgTec"_H:      return Detail::AssignEnumValue( EGPUVendorID::ImgTec, o_Value );
		case "Nvidia"_H:      return Detail::AssignEnumValue( EGPUVendorID::Nvidia, o_Value );
		case "Arm"_H:         return Detail::AssignEnumValue( EGPUVendorID::Arm, o_Value );
		case "Broadcom"_H:    return Detail::AssignEnumValue( EGPUVendorID::Broadcom, o_Value );
		case "Qualcomm"_H:    return Detail::AssignEnumValue( EGPUVendorID::Qualcomm, o_Value );
		case "Intel"_H:       return Detail::AssignEnumValue( EGPUVendorID::Intel, o_Value );
		case "Apple"_H:       return Detail::AssignEnumValue( EGPUVendorID::Apple, o_Value );
		case "Vivante"_H:     return Detail::AssignEnumValue( EGPUVendorID::Vivante, o_Value );
		case "VeriSilicon"_H: return Detail::AssignEnumValue( EGPUVendorID::VeriSilicon, o_Value );
		case "SamsungAMD"_H:  return Detail::AssignEnumValue( EGPUVendorID::SamsungAMD, o_Value );
		case "Microsoft"_H:   return Detail::AssignEnumValue( EGPUVendorID::Microsoft, o_Value );
		}
		return false;
	}

	//=========================================================
	// ERHIShaderModel
	//=========================================================

	static constexpr StringView ToString( ERHIShaderModel a_Model )
	{
		switch ( a_Model )
		{
		case ERHIShaderModel::Unknown: return "Unknown";
		case ERHIShaderModel::SM_5_0:  return "SM_5_0";
		case ERHIShaderModel::SM_6_0:  return "SM_6_0";
		case ERHIShaderModel::SM_6_1:  return "SM_6_1";
		case ERHIShaderModel::SM_6_2:  return "SM_6_2";
		case ERHIShaderModel::SM_6_3:  return "SM_6_3";
		case ERHIShaderModel::SM_6_4:  return "SM_6_4";
		case ERHIShaderModel::SM_6_5:  return "SM_6_5";
		case ERHIShaderModel::SM_6_6:  return "SM_6_6";
		default:                       return "<INVALID>";
		}
	}

	//=========================================================
	// ERHIFeatureSupport
	//=========================================================
	
	static constexpr StringView ToString( ERHIFeatureSupport a_Support )
	{
		switch ( a_Support )
		{
		case ERHIFeatureSupport::Unsupported: return "Unsupported";
		case ERHIFeatureSupport::Supported:   return "Supported";
		//case ERHIFeatureSupport::Optional:    return "Optional";
		default:                              return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_String, ERHIFeatureSupport& o_Value )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( a_String.data() ), a_String.size() ) )
		{
		case "Unsupported"_H: return Detail::AssignEnumValue( ERHIFeatureSupport::Unsupported, o_Value );
		case "Supported"_H:   return Detail::AssignEnumValue( ERHIFeatureSupport::Supported, o_Value );
		}
		return false;
	}

	//=========================================================
	// ERHIFeature
	//=========================================================

	static constexpr StringView ToString( ERHIFeature a_Feature )
	{
		switch ( a_Feature )
		{
		case ERHIFeature::ComputeShaders:    return "ComputeShaders";
		case ERHIFeature::MeshShaders:       return "MeshShaders";
		case ERHIFeature::Tesselation:       return "Tesselation";
		case ERHIFeature::RayTracing:        return "RayTracing";
		case ERHIFeature::BindlessResources: return "BindlessResources";
		default:                             return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_String, ERHIFeature& o_Value )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( a_String.data() ), a_String.size() ) )
		{
		case "ComputeShaders"_H:    return Detail::AssignEnumValue( ERHIFeature::ComputeShaders, o_Value );
		case "MeshShaders"_H:   return Detail::AssignEnumValue( ERHIFeature::MeshShaders, o_Value );
		case "Tesselation"_H:       return Detail::AssignEnumValue( ERHIFeature::Tesselation, o_Value );
		case "RayTracing"_H:        return Detail::AssignEnumValue( ERHIFeature::RayTracing, o_Value );
		case "BindlessResources"_H: return Detail::AssignEnumValue( ERHIFeature::BindlessResources, o_Value );
		}
		return false;
	}

	//=========================================================
	// ERHIResourceType
	//==========================================================

	static constexpr StringView ToString( ERHIResourceType a_Type )
	{
		switch ( a_Type )
		{
		case ERHIResourceType::Sampler:               return "Sampler";
		case ERHIResourceType::Texture:               return "Texture";
		case ERHIResourceType::ShaderModule:          return "ShaderModule";
		case ERHIResourceType::Buffer:                return "Buffer";
		case ERHIResourceType::BindingLayout:         return "BindingLayout";
		case ERHIResourceType::BindingSet:            return "BindingSet";
		case ERHIResourceType::GraphicsPipelineState: return "GraphicsPipelineState";
		case ERHIResourceType::ComputePipelineState:  return "ComputePipelineState";
		case ERHIResourceType::CommandList:           return "CommandList";
		case ERHIResourceType::CommandAllocator:      return "CommandAllocator";
		case ERHIResourceType::SwapChain:             return "SwapChain";
		case ERHIResourceType::Fence:                 return "Fence";
		case ERHIResourceType::Unknown:               return "Unknown";
		default:                                      return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_String, ERHIResourceType& o_Value )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( a_String.data() ), a_String.size() ) )
		{
		case "Sampler"_H:               return Detail::AssignEnumValue( ERHIResourceType::Sampler, o_Value );
		case "Texture"_H:               return Detail::AssignEnumValue( ERHIResourceType::Texture, o_Value );
		case "ShaderModule"_H:          return Detail::AssignEnumValue( ERHIResourceType::ShaderModule, o_Value );
		case "Buffer"_H:                return Detail::AssignEnumValue( ERHIResourceType::Buffer, o_Value );
		case "BindingLayout"_H:         return Detail::AssignEnumValue( ERHIResourceType::BindingLayout, o_Value );
		case "BindingSet"_H:            return Detail::AssignEnumValue( ERHIResourceType::BindingSet, o_Value );
		case "GraphicsPipelineState"_H: return Detail::AssignEnumValue( ERHIResourceType::GraphicsPipelineState, o_Value );
		case "ComputePipelineState"_H:  return Detail::AssignEnumValue( ERHIResourceType::ComputePipelineState, o_Value );
		case "CommandList"_H:           return Detail::AssignEnumValue( ERHIResourceType::CommandList, o_Value );
		case "CommandAllocator"_H:      return Detail::AssignEnumValue( ERHIResourceType::CommandAllocator, o_Value );
		case "SwapChain"_H:             return Detail::AssignEnumValue( ERHIResourceType::SwapChain, o_Value );
		case "Fence"_H:                 return Detail::AssignEnumValue( ERHIResourceType::Fence, o_Value );
		}

		return false;
	}

	//=========================================================
	// ERHIBindFlags
	//=========================================================

	static String ToString( ERHIBindFlags a_Flags )
	{
		constexpr Pair<ERHIBindFlags, StringView> BindFlagNames[] =
		{
			{ ERHIBindFlags::VertexBuffer,    "VertexBuffer" },
			{ ERHIBindFlags::IndexBuffer,     "IndexBuffer" },
			{ ERHIBindFlags::ConstantBuffer,  "ConstantBuffer" },
			{ ERHIBindFlags::ShaderResource,  "ShaderResource" },
			{ ERHIBindFlags::RenderTarget,    "RenderTarget" },
			{ ERHIBindFlags::DepthStencil,    "DepthStencil" },
			{ ERHIBindFlags::UnorderedAccess, "UnorderedAccess" }
		};
		return Detail::FlagsToString( a_Flags, Span( BindFlagNames ) );
	}

	static constexpr bool FromString( StringView a_Flags, ERHIBindFlags& o_Value )
	{
		o_Value = ERHIBindFlags::None;

		size_t start = 0;
		while ( start < a_Flags.size() )
		{
			// Find the next delimiter
			size_t end = a_Flags.find( '|', start );
			if ( end == StringView::npos )
				end = a_Flags.size();

			// Trim and extract substring
			StringView token = a_Flags.substr( start, end - start );

			switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( token.data() ), token.size() ) )
			{
			case "None"_H:             o_Value |= ERHIBindFlags::None; break;
			case "VertexBuffer"_H:     o_Value |= ERHIBindFlags::VertexBuffer; break;
			case "IndexBuffer"_H:      o_Value |= ERHIBindFlags::IndexBuffer; break;
			case "ConstantBuffer"_H:   o_Value |= ERHIBindFlags::ConstantBuffer; break;
			case "ShaderResource"_H:   o_Value |= ERHIBindFlags::ShaderResource; break;
			case "RenderTarget"_H:     o_Value |= ERHIBindFlags::RenderTarget; break;
			case "DepthStencil"_H:     o_Value |= ERHIBindFlags::DepthStencil; break;
			case "UnorderedAccess"_H:  o_Value |= ERHIBindFlags::UnorderedAccess; break;
			case "IndirectArgument"_H: o_Value |= ERHIBindFlags::IndirectArgument; break;
			}

			start = end + 1;
		}

		return true;
	}

	//=========================================================
	// ERHIUsage
	//=========================================================

	static String ToString( ERHIUsage a_Usage )
	{
		switch ( a_Usage )
		{
		case ERHIUsage::Default:  return "Default";
		case ERHIUsage::Static:   return "Static";
		case ERHIUsage::Dynamic:  return "Dynamic";
		default:                  return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_Usage, ERHIUsage& o_Value )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( a_Usage.data() ), a_Usage.size() ) )
		{
		case "Default"_H:  return Detail::AssignEnumValue( ERHIUsage::Default, o_Value );
		case "Static"_H:   return Detail::AssignEnumValue( ERHIUsage::Static, o_Value );
		case "Dynamic"_H:  return Detail::AssignEnumValue( ERHIUsage::Dynamic, o_Value );
		}
		return false;
	}
}