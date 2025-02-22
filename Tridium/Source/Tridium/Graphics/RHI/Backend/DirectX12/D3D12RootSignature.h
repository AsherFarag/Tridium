#pragma once
#include "D3D12Common.h"

namespace Tridium {

	struct D3D12RootSignature
	{
		ComPtr<ID3D12RootSignature> RootSig;
		D3D12_ROOT_SIGNATURE_DESC Desc;
	};

	// TEMP API
	extern UnorderedMap<String, SharedPtr<D3D12RootSignature>> s_RootSignatures;

}