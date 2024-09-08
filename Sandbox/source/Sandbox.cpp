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

class ExampleLayer : public Tridium::Layer
{

public:

	ExampleLayer() = default;

	virtual void OnImGuiDraw() override
	{
		//ImGui::ShowDemoWindow();
	}
};








struct FirstBase
{
	REFLECT;
	int firstBaseVar = 5;
};

struct SecondBase
{
	REFLECT;
	float secondBaseVar = 9.031f;
};

struct Nested
{
	REFLECT;
	int varA = 10;
	float varB = 5.5f;
	const char* varC = "Hello World";
	SecondBase varD;
};

struct Derived : public FirstBase, public SecondBase
{
	REFLECT;
	int derivedVar = 10;
	Nested nested;
	SecondBase secondBase;
};

BEGIN_REFLECT( FirstBase )
    PROPERTY( firstBaseVar )
END_REFLECT

BEGIN_REFLECT( SecondBase )
    PROPERTY( secondBaseVar )
END_REFLECT

BEGIN_REFLECT( Nested )
    PROPERTY( varA )
    PROPERTY( varB )
    PROPERTY( varC )
	PROPERTY( varD )
END_REFLECT

BEGIN_REFLECT( Derived )
    BASE( FirstBase )
	BASE( SecondBase )
    PROPERTY( derivedVar )
    PROPERTY( nested )
	PROPERTY( secondBase )
END_REFLECT

void Test()
{
	Derived test;
	YAML::Emitter out;
	Tridium::Reflection::MetaRegistry::WriteObjectToEmitter( out, "Test", test );

	std::ofstream outFile( "Content/testrefl.yaml" );
	outFile << out.c_str();
}











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