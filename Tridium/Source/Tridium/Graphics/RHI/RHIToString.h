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
	// ERHIObjectType
	//==========================================================

	static constexpr StringView ToString( ERHIObjectType a_Type )
	{
		switch ( a_Type )
		{
		case ERHIObjectType::Texture:               return "Texture";
		case ERHIObjectType::ShaderModule:          return "ShaderModule";
		case ERHIObjectType::Buffer:                return "Buffer";
		case ERHIObjectType::BindingLayout:         return "BindingLayout";
		case ERHIObjectType::BindingSet:            return "BindingSet";
		case ERHIObjectType::GraphicsPipelineState: return "GraphicsPipelineState";
		case ERHIObjectType::ComputePipelineState:  return "ComputePipelineState";
		case ERHIObjectType::CommandList:           return "CommandList";
		case ERHIObjectType::SwapChain:             return "SwapChain";
		case ERHIObjectType::Unknown:               return "Unknown";
		default:                                      return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_String, ERHIObjectType& o_Value )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>( a_String.data() ), a_String.size() ) )
		{
		case "Texture"_H:               return Detail::AssignEnumValue( ERHIObjectType::Texture, o_Value );
		case "ShaderModule"_H:          return Detail::AssignEnumValue( ERHIObjectType::ShaderModule, o_Value );
		case "Buffer"_H:                return Detail::AssignEnumValue( ERHIObjectType::Buffer, o_Value );
		case "BindingLayout"_H:         return Detail::AssignEnumValue( ERHIObjectType::BindingLayout, o_Value );
		case "BindingSet"_H:            return Detail::AssignEnumValue( ERHIObjectType::BindingSet, o_Value );
		case "GraphicsPipelineState"_H: return Detail::AssignEnumValue( ERHIObjectType::GraphicsPipelineState, o_Value );
		case "ComputePipelineState"_H:  return Detail::AssignEnumValue( ERHIObjectType::ComputePipelineState, o_Value );
		case "CommandList"_H:           return Detail::AssignEnumValue( ERHIObjectType::CommandList, o_Value );
		case "SwapChain"_H:             return Detail::AssignEnumValue( ERHIObjectType::SwapChain, o_Value );
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
	// ERHIFormat
	//=========================================================

	static constexpr StringView ToString( ERHIFormat a_Format )
	{
		return GetRHIFormatInfo( a_Format ).Name;
	}

	//=========================================================
	// ERHISamplerFilter
	//=========================================================

	static constexpr StringView ToString( ERHISamplerFilter a_Filter )
	{
		using enum ERHISamplerFilter;
		switch ( a_Filter )
		{
		case Unknown:                           	return "Unknown";
		case MinMagMipPoint:						return "MinMagMipPoint";
		case MinMagPointMipLinear:					return "MinMagPointMipLinear";
		case MinPointMagLinearMipPoint:				return "MinPointMagLinearMipPoint";
		case MinPointMagMipLinear:					return "MinPointMagMipLinear";
		case MinLinearMagMipPoint:					return "MinLinearMagMipPoint";
		case MinLinearMagPointMipLinear:			return "MinLinearMagPointMipLinear";
		case MinMagLinearMipPoint:					return "MinMagLinearMipPoint";
		case MinMagMipLinear:						return "MinMagMipLinear";
		case Anisotropic:							return "Anisotropic";
		case ComparisonMinMagMipPoint:				return "ComparisonMinMagMipPoint";
		case ComparisonMinMagPointMipLinear:		return "ComparisonMinMagPointMipLinear";
		case ComparisonMinPointMagLinearMipPoint:	return "ComparisonMinPointMagLinearMipPoint";
		case ComparisonMinPointMagMipLinear:		return "ComparisonMinPointMagMipLinear";
		case ComparisonMinLinearMagMipPoint:		return "ComparisonMinLinearMagMipPoint";
		case ComparisonMinLinearMagPointMipLinear:	return "ComparisonMinLinearMagPointMipLinear";
		case ComparisonMinMagLinearMipPoint:		return "ComparisonMinMagLinearMipPoint";
		case ComparisonMinMagMipLinear:				return "ComparisonMinMagMipLinear";
		case ComparisonAnisotropic:                 return "ComparisonAnisotropic";
		default:                                    return "<INVALID>";
		}
	}

	//=========================================================
	// ERHIBindingType
	//=========================================================

	static constexpr StringView ToString( ERHIBindingType a_Type )
	{
		switch ( a_Type )
		{
		case ERHIBindingType::Unknown:              return "Unknown";
		case ERHIBindingType::InlinedConstants:     return "InlinedConstants";
		case ERHIBindingType::ConstantBuffer:       return "ConstantBuffer";
		case ERHIBindingType::StructuredBuffer:     return "StructuredBuffer";
		case ERHIBindingType::StorageBuffer:        return "StorageBuffer";
		case ERHIBindingType::Texture:              return "Texture";
		case ERHIBindingType::StorageTexture:       return "StorageTexture";
		case ERHIBindingType::BindlessTextureArray: return "BindlessTextureArray";
		default:                                    return "<INVALID>";
		}
	}

	//=========================================================
	// ERHIDescriptorHeapType
	//=========================================================

	static constexpr StringView ToString( ERHIDescriptorHeapType a_Type )
	{
		switch ( a_Type )
		{
			case ERHIDescriptorHeapType::Sampler:         return "Sampler";
			case ERHIDescriptorHeapType::RenderResource:  return "RenderResource";
			case ERHIDescriptorHeapType::RenderTarget:    return "RenderTarget";
			case ERHIDescriptorHeapType::DepthStencil:    return "DepthStencil";
			default:                                      return "<INVALID>";
		}
	}
}