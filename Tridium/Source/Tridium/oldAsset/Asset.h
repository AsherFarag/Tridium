#pragma once
#include <string>
#include "AssetType.h"
#include <memory>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

#define ASSET_CLASS_TYPE(type) static constexpr EAssetTypeOld StaticType() { return EAssetTypeOld::type; }\
							   virtual EAssetTypeOld AssetType() const { return StaticType(); }\

	class Asset : public std::enable_shared_from_this<Asset>
    {
    public:
        ASSET_CLASS_TYPE( None );
		virtual ~Asset() = default;

        OldAssetHandle GetHandle() const { return m_Handle; }
		void SetHandle( OldAssetHandle a_Handle ) { m_Handle = a_Handle; }

    protected:
        OldAssetHandle m_Handle;

        friend class RuntimeAssetManager;
        friend class EditorAssetManager;
    };
}