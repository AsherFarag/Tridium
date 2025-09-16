#pragma once
#include <Tridium/Core/Config.h>

#if USE_ASSET_IMPORTERS

#include <Tridium/Asset/AssetDefinitions.h>
#include <Tridium/Asset/AssetDatabase.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Core/Version.h>
#include <Tridium/IO/FilePath.h>
#include <Tridium/Utils/StaticInitializer.h>

namespace Tridium {

	#define DEFINE_ASSET_IMPORTER( _Class ) \
		class _Class final : public ::Tridium::IAssetImporter

	//=================================================================================================
	// Registered an Asset Importer to the Asset Importer Factory.
	#define REGISTER_ASSET_IMPORTER( _Class ) \
		DECLARE_INITIALIZER( AssetImporter_##_Class ) \
		DEFINE_INITIALIZER( AssetImporter_##_Class ) \
		{ \
			::Tridium::AssetFactory::RegisterImporter<_Class>(); \
		}

	//=================================================================================================
	// Asset Import Context
	//=================================================================================================
	class AssetImportContext
	{
	public:

		//=============================================================================================
		AssetImportContext() = default;

		//=============================================================================================
		// The path of the source asset file to be imported.
		const FilePath& AssetPath() const { return m_AssetPath; }

		//=============================================================================================
		// The destination path where the imported assets should be saved.
		const FilePath& DestinationPath() const { return m_DestinationPath; }

		//=============================================================================================
		// The loaded data of the files contents.
		Span<const byte_t> FileData() const { return m_FileData; }

		//=============================================================================================
		// Logs a non-fatal warning message that occurred during the import process.
		// These warnings should be a performance or quality concern that the user should be aware of,
		// but do not prevent the asset from being imported successfully.
		void LogWarning( String a_Message ) { m_Warnings.EmplaceBack( std::move( a_Message ) ); }

		//=============================================================================================
		// Logs a non-fatal warning message that occurred during the import process.
		// These warnings should be a performance or quality concern that the user should be aware of,
		// but do not prevent the asset from being imported successfully.
		template<typename... _Args>
		inline void LogWarning( StringView a_Fmt, const _Args&... a_Args )
		{
			m_Warnings.EmplaceBack( std::vformat( a_Fmt, std::make_format_args( std::forward<const _Args>( a_Args )... ) ) );
		}
		//=============================================================================================
		// Returns the list of warnings that were logged during the import process.
		const auto& Warnings() const { return m_Warnings; }

		//=============================================================================================
		// Returns true if an error occured and the import operation has failed.
		bool ImportFailed() const { return m_HasFailed; }

		//=============================================================================================
		// If the import operation has failed, this will contain the error message.
		const String& ErrorMessage() const { return m_ErrorMessage; }

		//=============================================================================================
		// This cancels the import operation and logs the specified error message.
		void FailImport( String a_ErrorMessage )
		{
			ASSERT( !m_HasFailed, "Import has already failed" );
			m_HasFailed = true;
			m_ErrorMessage = std::move( a_ErrorMessage );
		}

		//=============================================================================================
		// Creates a new asset that will be serialized into its own file.
		template<Concepts::Derived<IAsset> T>
		AssetRef<T> CreateAsset( String a_Name, String a_Path, String a_SourceAssetPath = {} )
		{
			// When an importer creates an asset, we don't immediately register it with the AssetDatabase.
			// This is done so we can wait until the entire import process is complete and all assets are created.
			// So, if the import fails, we don't end up with partially imported assets in the database.

			AssetMetadata metaData
			{
				.ID = AssetID::Create(),
				.Type = T::StaticType(),
				.Name = std::move( a_Name ),
				.Path = std::move( a_Path ),
				.Editor
				{ 
					.SourceFilePath = a_SourceAssetPath.empty() ? m_AssetPath.ToString() : a_SourceAssetPath,
					.CreationTime = TimeStamp::Now(),
					.LastModifiedTime = TimeStamp::Now()
				}
			};

			AssetRef<T> asset = T::Create( metaData.ID );

			// Check if the asset was created successfully
			if ( !asset )
			{
				ASSERT( false, "Failed to create asset of type '{}' while importing '{}'.", GetTypeName<T>(), m_AssetPath.ToString() );
				return nullptr;
			}

			// Store the created asset and its metadata for later registration
			m_CreatedAssets.EmplaceBack( std::move( metaData ), asset );

			return asset;
		}

		//=============================================================================================
		// Registers a dependency between two assets.
		bool RegisterDependency( AssetID a_Dependent, AssetID a_Dependency )
		{
			if ( !a_Dependent.IsValid() || !a_Dependency.IsValid() )
			{
				ASSERT( false, "Invalid AssetID provided when registering dependency." );
				return false;
			}

			// Find the created asset that matches the dependent AssetID
			for ( auto& [meta, asset] : m_CreatedAssets )
			{
				if ( meta.ID == a_Dependent )
				{
					meta.Dependencies.insert( a_Dependency );
					return true;
				}
			}

			ASSERT( false, "Dependent asset with ID '{}' not found among created assets.", a_Dependent );
			return false;
		}

	public:

		//=============================================================================================
		FilePath m_AssetPath{};
		FilePath m_DestinationPath{};
		Span<const byte_t> m_FileData{};
		List<Pair<AssetMetadata, AssetRef<IAsset>>> m_CreatedAssets{};

		Array<String> m_Warnings{};
		bool m_HasFailed = false;
		String m_ErrorMessage{};

	};

	//=================================================================================================
	// Asset Importer Interface: Base class for all asset importers.
	// Handles a part of the asset import pipeline, converting raw source files into engine-ready assets.
	// To implement a custom importer, derive from this class and implement the OnImport method,
	// then register it with the AssetFactory using REGISTER_ASSET_IMPORTER( MyCustomImporter ) in a cpp file.
	//=================================================================================================
	class IAssetImporter
	{
	public:
		
		//=============================================================================================
		IAssetImporter() = default;
		virtual ~IAssetImporter() = default;

		//=============================================================================================
		// The importer version number that is used to determine if assets need to be re-imported.
		virtual VersionID Version() const { return 0; }

		//=============================================================================================
		// The list of file extensions (e.g. { "fbx", "png" } ) that this importer supports.
		virtual Array<StringView> SupportedExtensions() const = 0;

		//=============================================================================================
		//
		virtual bool OnImport( AssetImportContext& a_Context ) = 0;

	};

} // namespace Tridium

#endif // USE_ASSET_IMPORTERS