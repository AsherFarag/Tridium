#pragma once
#include <Tridium/Core/Hash.h>
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Containers/String.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/Map.h>

namespace Tridium {

	using DocumentationKey = hash_t;

	struct VariableDocumentation
	{
		String Name;
		String Description;
		String Type;
	};

	struct FunctionDocumentation
	{
		String Name;
		String Description;
		String ReturnType;
		Array<VariableDocumentation> Parameters;
	};

	struct TypeDocumentation
	{
		String Name;
		String Description;
		Array<VariableDocumentation> Members;
		Array<FunctionDocumentation> Functions;
	};

	class EditorDocumentation : public ISingleton<EditorDocumentation, /* _ExplicitSetup */ false>
	{
	public:
		DocumentationKey RegisterType( const TypeDocumentation& doc )
		{
			DocumentationKey key = Hashing::HashString( doc.Name.c_str() );
			m_Types[key] = doc;
			return key;
		}

		TypeDocumentation* GetType( DocumentationKey a_Key )
		{
			auto it = m_Types.find( a_Key );
			if ( it != m_Types.end() )
				return &it->second;
			return nullptr;
		}

		TypeDocumentation* GetType( const char* a_Name )
		{
			return GetType( Hashing::HashString( a_Name ) );
		}

	private:
		UnorderedMap<DocumentationKey, TypeDocumentation> m_Types;

	private:
		virtual void OnSingletonConstructed() override { Init(); }

		// Register all built-in types here
		void Init();
	};

}
