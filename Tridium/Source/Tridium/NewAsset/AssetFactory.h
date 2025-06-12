#pragma once
#include "Asset.h"
#include <Tridium/Core/Core.h>

namespace Tridium::T {

	// Forward declarations
	struct AssetMetadata;
	class IAssetLoader;

	using CreateAssetLoader = UniquePtr<IAssetLoader>(*)();

#define REGISTER_ASSET_LOADER( _Loader ) \
	namespace { \
		static struct StaticInitializer_##_Loader { \
			StaticInitializer_##_Loader() { \
				::Tridium::T::AssetFactory::s_AssetLoaderInitializers.PushBack( []() -> ::Tridium::UniquePtr<IAssetLoader> { \
					return ::Tridium::MakeUnique<_Loader>(); \
					} ); \
			} \
		} s_StaticInitializer_##_Loader; \
	}

	class IAssetLoader
	{
	public:
		virtual ~IAssetLoader() = default;
		virtual EAssetType GetAssetType() const = 0;
		virtual SharedPtr<IAsset> CreateAsset( AssetID a_ID, EAssetFlags a_Flags ) = 0;

		// Load and Save an Asset to/from a file.

		virtual Expected<void, String> Save( const SharedPtr<IAsset>& a_Asset, const AssetMetadata& a_Metadata ) = 0;
		virtual Expected<void, String> Load( SharedPtr<IAsset> a_Asset, const AssetMetadata& a_Metadata ) = 0;
	};

	template<Concepts::Derived<IAssetLoader> T>
	class AssetLoader : IAssetLoader
	{
	public:
		using AssetType = T;
		virtual ~AssetLoader() = default;
		[[nodiscard]] EAssetType GetAssetType() const override { return AssetType::Type; }
		[[nodiscard]] virtual SharedPtr<IAsset> CreateAsset( AssetID a_ID, EAssetFlags a_Flags ) override { return MakeShared<AssetType>( a_ID, a_Flags ); }
	};

	class AssetFactory
	{
	public:
		static Array<CreateAssetLoader> s_AssetLoaderInitializers;

		AssetFactory()
		{
			ASSERT( !s_AssetLoaderInitializers.Empty(),
				"No asset loaders are registered - are you attempting to statically initialize the AssetFactory?" );
			for ( CreateAssetLoader initializer : s_AssetLoaderInitializers )
			{
				UniquePtr<IAssetLoader> loader = initializer();
				if ( loader )
				{
					EAssetType assetType = loader->GetAssetType();
					RegisterLoader( assetType, std::move( loader ) );
				}
			}
		}

		void RegisterLoader( EAssetType a_Type, UniquePtr<IAssetLoader> a_Loader )
		{
			m_Loaders[Cast<size_t>(a_Type)] = std::move( a_Loader );
		}

		void UnregisterLoader( EAssetType a_Type )
		{
			m_Loaders[Cast<size_t>(a_Type)] = nullptr;
		}
		
		bool HasLoader( EAssetType a_Type ) const
		{
			return m_Loaders[Cast<size_t>(a_Type)] != nullptr;
		}

		IAssetLoader* GetLoader( EAssetType a_Type ) const
		{
			return m_Loaders[Cast<size_t>(a_Type)].get();
		}

	private:
		FixedArray<UniquePtr<IAssetLoader>, (size_t)EAssetType::COUNT> m_Loaders;
	};

} // namespace Tridium