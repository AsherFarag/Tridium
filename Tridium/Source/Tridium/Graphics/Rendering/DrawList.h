#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Asset/AssetType.h>

namespace Tridium { 
	struct DrawCommandKey
	{
		StaticMeshHandle Mesh;
		uint32_t SubMeshIndex;
		MaterialHandle Material;

		bool operator==( const DrawCommandKey& a_Other ) const
		{
			return Mesh == a_Other.Mesh && SubMeshIndex == a_Other.SubMeshIndex && Material == a_Other.Material;
		}
	};
}

namespace std {
	template <>
	struct hash<Tridium::DrawCommandKey>
	{
		std::size_t operator()( const Tridium::DrawCommandKey& key ) const noexcept
		{
			size_t seed = 0;
			Tridium::HashCombine( seed, std::hash<Tridium::GUID>{}( key.Mesh.ID() ) );
			Tridium::HashCombine( seed, std::hash<uint32_t>{}( key.SubMeshIndex ) );
			Tridium::HashCombine( seed, std::hash<Tridium::GUID>{}( key.Material.ID() ) );
			return seed;
		}
	};
}

namespace Tridium {

	// Forward declarations
	class VertexArray;
	// -------------------

	enum class EDrawPass : uint8_t
	{
		Opaque = BIT( 0 ),
		Transparent = BIT( 1 ),
		Shadows = BIT( 2 ),
		LAST
	};

	using DrawPass = EnumFlags<EDrawPass>;

	struct DrawCommand 
	{ 
		SharedPtr<VertexArray> VAO;
		std::vector<Matrix4> Transforms;
	};

	using DrawCommandList = std::unordered_map<DrawCommandKey, DrawCommand>;

	class DrawList
	{
	public:
        DrawList();

		void Clear();
		void AddCommand( DrawPass a_DrawPassFlags, StaticMeshHandle a_Mesh, const std::vector<MaterialHandle>& a_OverrideMaterials, const Matrix4& a_Transform );
		const DrawCommandList& GetCommandListByPass( EDrawPass a_DrawPass ) const { return m_CommandLists.find(a_DrawPass)->second; }

	private:
		std::unordered_map<EDrawPass, DrawCommandList> m_CommandLists;
	};

}