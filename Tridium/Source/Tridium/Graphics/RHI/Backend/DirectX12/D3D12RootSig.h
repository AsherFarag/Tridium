#pragma once
#include "D3D12.h"

namespace Tridium::D3D12 {

	ComPtr<ID3D12RootSignature> CreateRootSignature( const D3D12_ROOT_SIGNATURE_DESC1& a_Desc );

	struct DescriptorRange : public D3D12_DESCRIPTOR_RANGE1
	{
		constexpr explicit DescriptorRange() : D3D12_DESCRIPTOR_RANGE1() {}

		constexpr explicit DescriptorRange(
			D3D12_DESCRIPTOR_RANGE_TYPE a_RangeType,
			uint32_t a_NumDescriptors,
			uint32_t a_BaseShaderRegister, uint32_t a_RegisterSpace = 0,
			D3D12_DESCRIPTOR_RANGE_FLAGS a_Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE )
			: D3D12_DESCRIPTOR_RANGE1{ a_RangeType, a_NumDescriptors, a_BaseShaderRegister, a_RegisterSpace, a_Flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
		{
		}
	};

	struct RootParameter : public D3D12_ROOT_PARAMETER1
	{
		constexpr void AsConstants( uint32_t a_NumConstants, D3D12_SHADER_VISIBILITY a_ShaderVisibility, uint32_t a_ShaderRegister, uint32_t a_RegisterSpace = 0 )
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			ShaderVisibility = a_ShaderVisibility;
			Constants.Num32BitValues = a_NumConstants;
			Constants.ShaderRegister = a_ShaderRegister;
			Constants.RegisterSpace = a_RegisterSpace;
		}

		constexpr void AsCBV( D3D12_SHADER_VISIBILITY a_ShaderVisibility, uint32_t a_ShaderRegister, uint32_t a_RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS a_Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE )
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			ShaderVisibility = a_ShaderVisibility;
			Descriptor.ShaderRegister = a_ShaderRegister;
			Descriptor.RegisterSpace = a_RegisterSpace;
			Descriptor.Flags = a_Flags;
		}

		constexpr void AsSRV( D3D12_SHADER_VISIBILITY a_ShaderVisibility, uint32_t a_ShaderRegister, uint32_t a_RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS a_Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE )
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			ShaderVisibility = a_ShaderVisibility;
			Descriptor.ShaderRegister = a_ShaderRegister;
			Descriptor.RegisterSpace = a_RegisterSpace;
			Descriptor.Flags = a_Flags;
		}

		constexpr void AsUAV( D3D12_SHADER_VISIBILITY a_ShaderVisibility, uint32_t a_ShaderRegister, uint32_t a_RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS a_Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE )
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			ShaderVisibility = a_ShaderVisibility;
			Descriptor.ShaderRegister = a_ShaderRegister;
			Descriptor.RegisterSpace = a_RegisterSpace;
			Descriptor.Flags = a_Flags;
		}

		constexpr void AsDescriptorTable( D3D12_SHADER_VISIBILITY a_ShaderVisibility, Span<const DescriptorRange> a_Ranges )
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			ShaderVisibility = a_ShaderVisibility;
			DescriptorTable.NumDescriptorRanges = a_Ranges.size();
			DescriptorTable.pDescriptorRanges = a_Ranges.data();
		}
	};

	// Root Signature Description
	// A root signature is an array of root parameters, which are either root constants or root descriptors.
	// It can hold a maximum of 64 DWORDs (256 bytes) which is divided up between the root parameters.
	// Root constants are 32-bit values that are stored in the root signature and can be accessed by the shader.
	// Root descriptors (CBV, SRV, UAV) are descriptors that are stored in the root signature and can be accessed by the shader.
	// Root descriptors are 64-bit values that point to a descriptor in a descriptor heap.
	// Root descriptor tables are arrays of descriptors that are stored in the root signature and can be accessed by the shader.
	// Root descriptor tables are 64-bit values that point to a descriptor table in a descriptor heap.
	// Static samplers are compiled into the shader and do not need to be set by the application nor take up space in the root signature.
	struct RootSignatureDesc : public D3D12_ROOT_SIGNATURE_DESC1
	{
		constexpr RootSignatureDesc(
			Span<const RootParameter> a_Parameters,
			Span<const D3D12_STATIC_SAMPLER_DESC> a_StaticSamplers = {},
			D3D12_ROOT_SIGNATURE_FLAGS a_Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE )
			: D3D12_ROOT_SIGNATURE_DESC1{
				static_cast<UINT>( a_Parameters.size() ), a_Parameters.data(),
				static_cast<UINT>( a_StaticSamplers.size() ), a_StaticSamplers.data(),
				a_Flags }
		{
		}

		ComPtr<ID3D12RootSignature> Create() const
		{
			return CreateRootSignature( *this );
		}
	};

}