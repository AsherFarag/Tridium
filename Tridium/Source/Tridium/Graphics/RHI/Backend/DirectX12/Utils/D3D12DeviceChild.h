#pragma once
#include "../D3D12.h"
#include "D3D12Device.h"

namespace Tridium::D3D12 {

	class DeviceChild
	{
	public:
		NON_COPYABLE( DeviceChild );
		DeviceChild() = delete;
		DeviceChild( Device* a_ParentDevice )
			: m_ParentDevice( a_ParentDevice ) {}
		DeviceChild( DeviceChild&& other ) noexcept
			: m_ParentDevice( std::move( other.m_ParentDevice ) ) {}
		DeviceChild& operator=( DeviceChild&& other ) noexcept
		{
			if ( this != &other )
			{
				m_ParentDevice = std::move( other.m_ParentDevice );
			}
			return *this;
		}
		virtual ~DeviceChild() = default;
		Device* ParentDevice() const { return m_ParentDevice; }

	private:
		Device* m_ParentDevice;
	};

}