#include "tripch.h"
#include "D3D12RootSignature.h"

namespace Tridium {
	UnorderedMap<String, SharedPtr<D3D12RootSignature>> s_RootSignatures;
}