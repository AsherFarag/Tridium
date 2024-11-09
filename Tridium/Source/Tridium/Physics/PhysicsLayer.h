#pragma once
#include <Tridium/Core/Core.h>
#include <bitset>

namespace Tridium {

	enum class EPhysicsLayer : uint8_t
	{
		Layer0 = 0,
		Layer1,
		Layer2,
		Layer3,
		Layer4,
		Layer5,
		Layer6,
		Layer7,
		Layer8,
		Layer9,
		NUM_LAYERS,
		INVALID = 0xFF
	};

	using PhysicsLayerMask = std::bitset<static_cast<size_t>( EPhysicsLayer::NUM_LAYERS )>;

	class PhysicsLayerManager
	{
	public:
		EPhysicsLayer GetLayerFromName( const std::string& a_LayerName ) const
		{
			auto it = m_LayerNames.find( a_LayerName );
			return it != m_LayerNames.end() ? it->second : EPhysicsLayer::INVALID;
		}

		std::string_view GetLayerName( EPhysicsLayer a_Layer ) const
		{
			for ( auto& [name, layer] : m_LayerNames )
				if ( layer == a_Layer ) return name;

			return {};
		}

		const auto& GetLayerNames() const { return m_LayerNames; }

		const PhysicsLayerMask& GetLayerMask( EPhysicsLayer a_Layer ) const
		{
			return m_LayerMasks.at( a_Layer );
		}

		void SetLayerMask( EPhysicsLayer a_Layer, const PhysicsLayerMask& a_Mask )
		{
			m_LayerMasks[a_Layer] = a_Mask;
		}

		bool IsLayerInMask( EPhysicsLayer a_Layer, const PhysicsLayerMask& a_Mask ) const
		{
			return a_Mask.test( static_cast<size_t>( a_Layer ) );
		}
		 
	private:
		std::unordered_map<EPhysicsLayer, PhysicsLayerMask> m_LayerMasks;
		std::unordered_map<std::string, EPhysicsLayer> m_LayerNames;
	};

} // namespace Tridium