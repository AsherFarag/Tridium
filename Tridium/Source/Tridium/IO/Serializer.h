#pragma once
#include "FilePath.h"
#include <Tridium/Core/Core.h>
#include <Tridium/Utils/Todo.h>

namespace Tridium::IO {

	//template<typename T>
	//void SerializeToArchive( IO::Archive& a_Archive, const T& a_Data )
	//{
	//	using SerFunc = Refl::Internal::SerializeFunc;

	//	// Get the serialize function from the meta registry only once by using a static lambda call.
	//	static SerFunc s_SerializeFunc = [] {
	//		SerFunc temp = nullptr;
	//		if ( !Refl::MetaRegistry::TryGetMetaPropertyFromClass<T>( temp, Refl::Internal::YAMLSerializeFuncID ) )
	//			TE_CORE_ASSERT( false );

	//		return temp;

	//	}( );

	//	s_SerializeFunc( a_Archive, a_Data );
	//}

	//template<typename T>
	//bool DeserializeFromArchive( const YAML::Node& a_Archive, T& o_Data )
	//{
	//	using DeserFunc = Refl::Internal::DeserializeFunc;

	//	static const Refl::MetaType s_MetaType = Refl::MetaRegistry::ResolveMetaType<T>();

	//	// Get the deserialize function from the meta registry only once by using a static lambda call.
	//	static DeserFunc s_DeserializeFunc = [] {
	//		DeserFunc temp = nullptr;
	//		if ( !Refl::MetaRegistry::TryGetMetaPropertyFromClass<T>( temp, Refl::Internal::YAMLDeserializeFuncID ) )
	//			TE_CORE_ASSERT( false );

	//		return temp;

	//		}( );

	//	Refl::MetaAny dataAny = s_MetaType.from_void( &o_Data );
	//	s_DeserializeFunc( a_Archive, dataAny );

	//	TODO( "Implement a proper return value" );
	//	return true;
	//}

	//template<typename T>
	//bool DeserializeFromFilePath( const FilePath& a_Path, T& o_Data )
	//{
	//	YAML::Node archive;

	//	try
	//	{
	//		archive = YAML::LoadFile( a_Path );
	//	}
	//	catch ( const YAML::BadFile& e )
	//	{
	//		TE_CORE_ERROR( "Failed to load file '{0}'! Error: {1}", a_Path.ToString(), e.what());
	//		return false;
	//	}

	//	return DeserializeFromArchive( archive, o_Data );
	//}

}