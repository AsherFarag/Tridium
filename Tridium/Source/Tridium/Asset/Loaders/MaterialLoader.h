#pragma once
namespace Tridium {

	class Material;
	struct MaterialMetaData;

	class MaterialLoader
	{
	public:
		static Material* Load( const IO::FilePath& a_Path, const MaterialMetaData& a_MetaData );
	};

}