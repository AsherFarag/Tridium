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
#include <Tridium/IO/TextSerializer.h>

#include <any>

struct Nested
{
	REFLECT;
	int NestedVarA = 10;
	float NestedVarB = 5.5f;
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
	Vector2 vec2;
	Vector3 vec3;
	Vector4 vec4;
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
	//Tridium::Refl::MetaRegistry::WriteObjectToEmitter( out, "derivedInstance", derivedInstance );

	std::map<std::string, std::vector< int >> map = { {"MyInt1", {1,2,3,4,5,6}}, {"MyInt2", {1,2,3,4,5,6}}, {"MyInt3", {1,2,3,4,5,6}} };

	out << YAML::BeginMap;

	out << YAML::Key << "MyMap" << YAML::Value;

	IO::SerializeToText( out, map );

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
    #ifndef IS_EDITOR
		SceneSerializer serializer( Application::GetScene() );
		serializer.DeserializeText( ( Application::GetAssetDirectory() / "Scene.tridium" ).string() );
    #endif // !IS_EDITOR
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