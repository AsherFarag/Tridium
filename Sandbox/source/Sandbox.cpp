#include <Tridium.h>
using namespace Tridium;
#include <Tridium/Rendering/Material.h>
#include <Tridium/IO/MaterialSerializer.h>
#include <Tridium/IO/SceneSerializer.h>
#include <variant>
#include "Editor/Editor.h"
#include <Tridium/IO/SerializationUtil.h>
#include <Tridium/Asset/AssetMetaData.h>
#include <Tridium/Utils/Reflection/Reflection.h>

#include <any>

class ExampleComponent : public Tridium::Component
{
	REFLECT;
public:
	std::map<std::string, int> MyMap;
	std::unordered_map<std::string, int> MyUnorderedMap;
	std::vector<int> MyVec = { 1,2,3,4,5,6,7,8,9,10 };
	std::vector<Vector4> MyVectorOfVec4s = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12} };

};

BEGIN_REFLECT_COMPONENT( ExampleComponent )
PROPERTY( MyMap, FLAGS( Serialize, EditAnywhere ) )
PROPERTY( MyUnorderedMap, FLAGS( Serialize, EditAnywhere ) )
PROPERTY( MyVec, FLAGS( Serialize, EditAnywhere ) )
PROPERTY( MyVectorOfVec4s, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( ExampleComponent )


struct Nested
{
	REFLECT;

	int NestedVarA = 10;
	float NestedVarB = 5.5f;

	std::map<std::string, Vector3> MyMap = { {"MyVec1", {1,2,3}}, {"MyVec2", {1,2,3}}, {"MyVec3", {1,2,3}} };
	std::vector<int> MyVec = { 1,2,3,4,5,6,7,8,9,10 };
};

struct Base
{
	REFLECT;
	int BaseVar = 5;
	std::string BaseVar2 = "Hello";
};

struct Derived : public Base
{
	REFLECT;

	int DerivedVar = 10;
	float DerivedVar2 = 5.5f;
	bool DerivedVar3 = true;
	Vector2 vec2{0};
	Vector3 vec3{0};
	Vector4 vec4{0};
private:
	Vector2 mySuperLongVariableName;
	Nested nested;
};


BEGIN_REFLECT( Base )
	PROPERTY( BaseVar, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( BaseVar2, FLAGS( Serialize ) )
END_REFLECT( Base )

BEGIN_REFLECT( Nested )
	PROPERTY( NestedVarA, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( NestedVarB, FLAGS( Serialize, VisibleAnywhere ) )
	PROPERTY( MyMap, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( MyVec, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( Nested )

BEGIN_REFLECT( Derived )
	BASE( Base )
	PROPERTY( DerivedVar, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( DerivedVar2, FLAGS( Serialize, VisibleAnywhere ) )
	PROPERTY( DerivedVar3, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( vec2, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( vec3, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( vec4, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( nested, FLAGS( Serialize, VisibleAnywhere ) )
	PROPERTY( mySuperLongVariableName, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( Derived )


using namespace Tridium::Refl;

void Test()
{
	Derived derivedInstance;
	YAML::Emitter out;

	Internal::SerializeFunc serFunc;
	MetaRegistry::TryGetMetaPropertyFromClass<Derived>( serFunc, Internal::YAMLSerializeFuncID );

	//Tridium::Refl::MetaRegistry::WriteObjectToEmitter( out, "derivedInstance", derivedInstance );

	std::map<std::string, std::vector< int >> map = { {"MyInt1", {1,2,3,4,5,6}}, {"MyInt2", {1,2,3,4,5,6}}, {"MyInt3", {1,2,3,4,5,6}} };

	out << YAML::BeginMap;

	out << YAML::Key << "derivedInstance" << YAML::Value;

	serFunc( out, entt::forward_as_meta( derivedInstance ) );

	out << YAML::EndMap;

	std::ofstream outFile( "Content/testrefl.yaml" );
	outFile << out.c_str();

	//std::map<std::string, int> deserMap;
	//YAML::Node data = YAML::LoadFile( "Content/testrefl.yaml" ); 
	//IO::DeserializeFromText( data["MyMap"], deserMap );

	//for ( auto& it : deserMap )
	//{
	//	TE_CORE_TRACE( "{0} : {1}", it.first, it.second );
	//}
}

class ExampleLayer : public Tridium::Layer
{

public:

	ExampleLayer() = default;

	virtual void OnImGuiDraw() override
	{
	}
};


class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		PushLayer( new ExampleLayer() );

		Test();
	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}