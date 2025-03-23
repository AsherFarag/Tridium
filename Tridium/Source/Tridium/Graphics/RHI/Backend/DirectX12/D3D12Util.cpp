#include "tripch.h"
#include "D3D12Util.h"
#include "D3D12DynamicRHI.h"

namespace Tridium::D3D12 {

	ComPtr<ID3D12RootSignature> CreateRootSignature( const D3D12_ROOT_SIGNATURE_DESC1& a_Desc )
	{
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rsDesc.Desc_1_1 = a_Desc;

		HRESULT hr{ S_OK };
		ComPtr<ID3DBlob> serializedRootSig;
		ComPtr<ID3DBlob> errorBlob;
		hr = D3D12SerializeVersionedRootSignature( &rsDesc, &serializedRootSig, &errorBlob );
		if ( FAILED( hr ) )
		{
			if ( errorBlob )
			{
				LOG( LogCategory::DirectX, Error, "Failed to serialize root signature - Error: {0}", (char*)errorBlob->GetBufferPointer() );
			}
			else
			{
				LOG( LogCategory::DirectX, Error, "Failed to serialize root signature" );
			}
			return nullptr;
		}

		ComPtr<ID3D12RootSignature> rootSignature;
		hr = RHI::GetD3D12RHI()->GetDevice()->CreateRootSignature( 0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS( &rootSignature ) );
		if ( FAILED( hr ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create root signature" );
			return nullptr;
		}

		return rootSignature;
	}

}