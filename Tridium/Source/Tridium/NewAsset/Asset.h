#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/GUID.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Containers/String.h>

namespace Tridium::T {

	using AssetID = GUID; // Alias for asset ID, using GUID for unique identification

	//==============================
	// Asset flags
	//  Bit flags for indicating the state of an asset.
	enum class EAssetFlags : uint32_t
	{
		None = 0,
		MemoryOnly = 1 << 1, // The asset is only stored in memory, not on disk. This is set by the AssetDatabase.
	};
	ENUM_ENABLE_BITMASK_OPERATORS( EAssetFlags );

	//==============================
	// Asset types
	//  Enum of all asset types.
	enum class EAssetType : uint8_t
	{
		Unknown = 0,
		Texture,
		Material,
		MeshSource,
		StaticMesh,
		Sound,
		Scene,
		Script,
		Animation,
		Font,
		COUNT
	};

	enum class EAssetLoadPolicy
	{
		Default = 0,      // When the asset is requested, it will load either immediately or asynchronously based on the request.
		ImmediateOnly,    // When the asset is requested, it will always load immediately on the main thread.
		AsyncOnly,        // When the asset is requested, it will always load asynchronously.
		ImmediateOnStart, // When the asset is requested, it will load immediately on the main thread during the startup phase.
		AsyncOnStart,     // When the asset is requested, it will load asynchronously during the startup phase.
	};

	//==============================
	// Asset interface
	//  Base class for all assets.
	//  Assets are identified by a AssetID and are managed by the asset manager.
	class IAsset : public EnableSharedFromThis<IAsset>
	{
	public:
		NON_COPYABLE_OR_MOVABLE( IAsset );
		virtual ~IAsset() = default;

		auto ID() const { return m_AssetID; }
		auto Flags() const { return m_AssetFlags; }

		static constexpr auto StaticType() { return EAssetType::Unknown; }
		virtual EAssetType Type() const = 0;
		virtual bool Valid() const = 0;
		virtual void OnDependencyUpdated( const AssetID& a_DependencyID ) {}

		virtual bool operator==( const IAsset& a_Other ) const { return m_AssetID == a_Other.m_AssetID; }
		virtual bool operator!=( const IAsset& a_Other ) const { return !(*this == a_Other); }

	protected:
		AssetID m_AssetID{};
		EnumFlags<EAssetFlags> m_AssetFlags{ EAssetFlags::MemoryOnly };

	protected:
		IAsset() : m_AssetID( AssetID::Create() ), m_AssetFlags( EAssetFlags::MemoryOnly ) {}

		friend class AssetDatabase;
	};

	/////////////////////////////////////////////////////////////

	static constexpr EAssetType GetAssetTypeFromFileExtension( StringView a_Extension )
	{
		StringView ext = a_Extension[0] == '.' ? a_Extension.substr( 1 ) : a_Extension; // Remove leading dot if present
		switch ( Hashing::Util::HashAsLowerCaseAndTrimmed( ext ) )
		{
			// Tridium File Extensions
			case "tscene"_H:      return EAssetType::Scene;
			case "tmesh"_H:       return EAssetType::StaticMesh;
			case "tmat"_H:        return EAssetType::Material;

			// Mesh Sources
			case "obj"_H:         return EAssetType::MeshSource;
			case "fbx"_H:         return EAssetType::MeshSource;
			case "gltf"_H:        return EAssetType::MeshSource;

			// Textures
			case "png"_H:         return EAssetType::Texture;
			case "jpg"_H:         return EAssetType::Texture;
			case "jpeg"_H:        return EAssetType::Texture;
			case "tga"_H:         return EAssetType::Texture;
			case "bmp"_H:         return EAssetType::Texture;
			case "dds"_H:         return EAssetType::Texture;
			case "hdr"_H:         return EAssetType::Texture;

			// Audio

			// Scripts
			case "lua"_H:         return EAssetType::Script;
		}

		return EAssetType::Unknown; // Default case if no match found
	}

	static constexpr StringView ToString( EAssetType a_Type )
	{
		switch ( a_Type )
		{
		case EAssetType::Unknown:     return "Unknown";
		case EAssetType::Texture:     return "Texture";
		case EAssetType::Material:    return "Material";
		case EAssetType::MeshSource:  return "MeshSource";
		case EAssetType::StaticMesh:  return "StaticMesh";
		case EAssetType::Sound:       return "Sound";
		case EAssetType::Scene:       return "Scene";
		case EAssetType::Script:      return "Script";
		case EAssetType::Animation:   return "Animation";
		case EAssetType::Font:        return "Font";
		default:                      return "<INVALID>";
		}
	}

	static constexpr bool FromString( StringView a_String, EAssetType& o_Value )
	{
		switch ( Hashing::Util::HashAsLowerCaseAndTrimmed( a_String ) )
		{
		case "unknown"_H:     return (o_Value = EAssetType::Unknown, true );
		case "texture"_H:     return (o_Value = EAssetType::Texture, true );
		case "material"_H:    return (o_Value = EAssetType::Material, true );
		case "meshsource"_H:  return (o_Value = EAssetType::MeshSource, true );
		case "staticmesh"_H:  return (o_Value = EAssetType::StaticMesh, true );
		case "sound"_H:       return (o_Value = EAssetType::Sound, true );
		case "scene"_H:       return (o_Value = EAssetType::Scene, true );
		case "script"_H:      return (o_Value = EAssetType::Script, true );
		case "animation"_H:   return (o_Value = EAssetType::Animation, true );
		case "font"_H:        return (o_Value = EAssetType::Font, true );
		default:              return false;
		}
	}

} // namespace Tridium