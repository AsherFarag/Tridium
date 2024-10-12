#include <Tridium.h>

struct PersonInfo
{
	int Age = 0;
	std::string Name = "John Doe";
	float Height = 0.0f;
	bool IsMale = true;

	std::vector<std::string> Numbers = { "1", "2", "3" };
	std::map<std::string, std::string> Map = { { "Key1", "Value1" }, { "Key2", "Value2" } };
};

BEGIN_REFLECT( PersonInfo )
	PROPERTY( Age, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Name, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Height, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( IsMale, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Map, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( PersonInfo )

class Person : public Tridium::Component
{
	REFLECT(Person);
public:
	PersonInfo Info;
	std::map<std::string, PersonInfo> Children;
	std::vector<int> Numbers = { 1, 2, 3 };
};

BEGIN_REFLECT_COMPONENT( Person )
	PROPERTY( Info, FLAGS( EditAnywhere ) )
	PROPERTY( Children, FLAGS( EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( EditAnywhere ) )
END_REFLECT( Person )

#include <fstream>

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
		using namespace Tridium::Refl;
		PersonInfo info{ 10, "MyThing", 5, false};

		Internal::SerializeFunc func;
		if ( MetaRegistry::TryGetMetaPropertyFromClass<PersonInfo>( func, Internal::YAMLSerializeFuncID ) )
		{

			//Tridium::IO::Archive archive;
			//func( archive, entt::forward_as_meta( info ) );

			//std::ofstream file( "PersonInfo.yaml" );
			//file << archive.c_str();

			//file.close();

			auto node =  YAML::LoadFile( "PersonInfo.yaml" );

			Internal::DeserializeFunc deserializeFunc;
			if ( MetaRegistry::TryGetMetaPropertyFromClass<PersonInfo>( deserializeFunc, Internal::YAMLDeserializeFuncID ) )
			{
				MetaAny any = entt::forward_as_meta( info );
				deserializeFunc( node, any );

				LOG_DEBUG( "Deserialized PersonInfo: {0}, {1}, {2}, {3}", info.Age, info.Name, info.Height, info.IsMale );
				for ( const auto& number : info.Numbers )
				{
					LOG_DEBUG( "Number: {0}", number );
				}

				for ( const auto& [key, value] : info.Map )
				{
					LOG_DEBUG( "Map: {0} -> {1}", key, value );
				}
			}
		}
	}
};

Tridium::GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}