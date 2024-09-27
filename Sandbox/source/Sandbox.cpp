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

struct Nested
{
	REFLECT;
	int NestedVarA = 10;
	float NestedVarB = 5.5f;
	std::string NestedVarC = "I was serialized!";
};

struct Base
{
	REFLECT;
	int BaseVar = 5;
	Nested BaseNested;
};

struct Derived : public Base
{
	REFLECT;
	int DerivedVar = 10;
	const char* OtherDerivedVar = "I was not serialized!";
	Nested DerivedNested;
};


BEGIN_REFLECT( Base )
	PROPERTY( BaseVar, FLAGS( Serialize, VisibleAnywhere ) )
	PROPERTY( BaseNested, FLAGS( Serialize, VisibleAnywhere ) )
END_REFLECT( Base )

BEGIN_REFLECT( Nested )
	PROPERTY( NestedVarA, FLAGS( Serialize, VisibleAnywhere ) )
	PROPERTY( NestedVarB, FLAGS( Serialize, VisibleAnywhere ) )
	PROPERTY( NestedVarC, FLAGS( Serialize, VisibleAnywhere ) )
END_REFLECT( Nested )

BEGIN_REFLECT( Derived )
	BASE( Base )
	PROPERTY( DerivedVar, FLAGS( Serialize, VisibleAnywhere ) )
	PROPERTY( OtherDerivedVar, FLAGS( Serialize ) )
	PROPERTY( DerivedNested, FLAGS( Serialize ) )
END_REFLECT( Derived )

void Test()
{
	Derived derivedInstance;
	YAML::Emitter out;
	Tridium::Refl::MetaRegistry::WriteObjectToEmitter( out, "derivedInstance", derivedInstance );

	std::ofstream outFile( "Content/testrefl.yaml" );
	outFile << out.c_str();
}

class ExampleLayer : public Tridium::Layer
{

public:

	ExampleLayer() = default;

	Base derivedInstance;

	virtual void OnImGuiDraw() override
	{
		using namespace Tridium::Editor;
		auto drawFunc = +[]( const entt::meta_handle& handle ) {};

		if ( ImGui::Begin("test window") )
		{

			if ( ::Tridium::Refl::MetaRegistry::TryGetMetaPropertyFromClass<Base>( drawFunc, ::Tridium::Editor::Internal::DrawPropFuncID ) )
			{

				drawFunc( entt::meta_handle(derivedInstance));
			}

			ImGui::End();
		}
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