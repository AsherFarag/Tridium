#pragma once
#include "RenderResource.h"
#include <Tridium/Containers/UnorderedMap.h>
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	//===========================================================
	// Render Resource Manager
	//  Stores the render resources in a map which can be accessed by AssetID and VariantID.
	class RenderResourceManager : public ISingleton<RenderResourceManager, /*Explicit Setup*/ true, /*Is Owning*/ false>
	{
	public:
		template<Concepts::Derived<RenderResource> T>
		using ResourceVariants = SmallArray<T, 8>;

		template<Concepts::Derived<RenderResource> T>
		using ResourceMap = UnorderedMap<AssetID, ResourceVariants<T>>;

		// = Mesh Sources =

		RenderResourceMeshSource GetMeshSource( AssetID a_AssetID, RenderResourceID a_VariantID = 0 )
		{
			return GetResource( m_MeshSources, a_AssetID, a_VariantID );
		}

		bool AddMeshSource( const RenderResourceMeshSource& a_MeshSource, bool a_ForceReplace = false )
		{
			return AddResource( m_MeshSources, a_MeshSource, a_ForceReplace );
		}

		bool RemoveMeshSource( AssetID a_AssetID, RenderResourceID a_VariantID = c_InvalidRenderResourceID )
		{
			return RemoveResource( m_MeshSources, a_AssetID, a_VariantID );
		}

		// = Materials =

		RenderResourceMaterial GetMaterial( AssetID a_AssetID, RenderResourceID a_VariantID = 0 )
		{
			return GetResource( m_Materials, a_AssetID, a_VariantID );
		}

		bool AddMaterial( const RenderResourceMaterial& a_Material, bool a_ForceReplace = false )
		{
			return AddResource( m_Materials, a_Material, a_ForceReplace );
		}

		bool RemoveMaterial( AssetID a_AssetID, RenderResourceID a_VariantID = c_InvalidRenderResourceID )
		{
			return RemoveResource( m_Materials, a_AssetID, a_VariantID );
		}

		// = Textures =

		RenderResourceTexture GetTexture( AssetID a_AssetID, RenderResourceID a_VariantID = 0 )
		{
			return GetResource( m_Textures, a_AssetID, a_VariantID );
		}

		bool AddTexture( const RenderResourceTexture& a_Texture, bool a_ForceReplace = false )
		{
			return AddResource( m_Textures, a_Texture, a_ForceReplace );
		}

		bool RemoveTexture( AssetID a_AssetID, RenderResourceID a_VariantID = c_InvalidRenderResourceID )
		{
			return RemoveResource( m_Textures, a_AssetID, a_VariantID );
		}

		// = Utility Functions =

		// Clears all resources in the manager.
		void Clear()
		{
			m_MeshSources.clear();
			m_Materials.clear();
			m_Textures.clear();
		}

	private:
		ResourceMap<RenderResourceMeshSource> m_MeshSources{};
		ResourceMap<RenderResourceMaterial> m_Materials{};
		ResourceMap<RenderResourceTexture> m_Textures{};

		template<Concepts::Derived<RenderResource> T>
		auto GetResourceIterator( ResourceVariants<T>& a_Variants, RenderResourceID a_VariantID )
		{
			// Find the resource with the same variant ID as 'a_VariantID'.
			return std::find_if( a_Variants.Begin(), a_Variants.End(),
				[=]( const T& a_Resource ) -> bool
				{
					return a_Resource.VariantID == a_VariantID;
				} );
		}

		template<Concepts::Derived<RenderResource> T>
		T GetResource( ResourceMap<T>& a_ResourceMap, AssetID a_AssetID, RenderResourceID a_VariantID )
		{
			auto variantsIt = a_ResourceMap.find( a_AssetID );
			if ( variantsIt == a_ResourceMap.end() )
				return {}; // Return an invalid resource

			
			auto resourceIt = GetResourceIterator( variantsIt->second, a_VariantID );
			if ( resourceIt == variantsIt->second.End() )
				return {}; // Unable to find the specified variant, return an invalid resource.

			return *resourceIt;
		}

		template<Concepts::Derived<RenderResource> T>
		bool AddResource( ResourceMap<T>& a_ResourceMap, const T& a_Resource, bool a_ForceReplace )
		{
			ResourceVariants<T>& variants = a_ResourceMap[a_Resource.AssetID];
			auto resourceIt = GetResourceIterator( variants, a_Resource.VariantID );
			if ( resourceIt != variants.End() )
			{
				if ( !a_ForceReplace )
					return false; // Resource with the same variant ID already exists and replacement is not allowed.

				*resourceIt = a_Resource;
				return true; // Replaced the existing resource with the new one.
			}


			variants.PushBack(a_Resource);
			return true;
		}

		template<Concepts::Derived<RenderResource> T>
		bool RemoveResource( ResourceMap<T>& a_ResourceMap, AssetID a_AssetID, RenderResourceID a_VariantID )
		{
			auto variantsIt = a_ResourceMap.find( a_AssetID );
			if ( variantsIt == a_ResourceMap.end() )
				return false; // No resources for the asset.

			if ( a_VariantID == c_InvalidRenderResourceID )
			{
				// Remove all variants
				a_ResourceMap.erase( variantsIt );
				return true;
			}

			// Find the resource with the same variant ID as 'a_VariantID'.
			auto resourceIt = GetResourceIterator( variantsIt->second, a_VariantID );
			if ( resourceIt == variantsIt->second.End() )
				return false; // Unable to find the specified variant.

			if ( variantsIt->second.Size() == 1 )
			{
				// If this is the only variant, remove the entire asset entry.
				a_ResourceMap.erase( variantsIt );
				return true;
			}

			variantsIt->second.Erase( resourceIt );
			return true;
		}

	};

}  // namespace Tridium