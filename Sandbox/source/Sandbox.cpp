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

	Derived()
	{
		printf( "Derirved was constructed" );
	}

	int DerivedVar = 10;
	float DerivedVar2 = 5.5f;
	bool DerivedVar3 = true;
	Vector2 vec2;
	Vector3 vec3;
	Vector4 vec4;
	Nested nested;
};


BEGIN_REFLECT( Base )
	PROPERTY( BaseVar, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( BaseVar2, FLAGS( Serialize, VisibleAnywhere ) )
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
END_REFLECT( Derived )

void Test()
{
	Derived derivedInstance;
	YAML::Emitter out;
	Tridium::Refl::MetaRegistry::WriteObjectToEmitter( out, "derivedInstance", derivedInstance );

	std::ofstream outFile( "Content/testrefl.yaml" );
	outFile << out.c_str();

	auto derivedVarMeta = entt::resolve<Derived>().data(entt::hashed_string("DerivedVar"));
	derivedVarMeta.set( derivedInstance, 20 );
	printf( "DerivedVar: %d\n", derivedInstance.DerivedVar );
}

class ExampleLayer : public Tridium::Layer
{

public:

	ExampleLayer() = default;

	Derived derivedInstance;

	virtual void OnImGuiDraw() override
	{
		using namespace Tridium::Editor;
		::Tridium::Editor::Internal::DrawPropFunc drawFunc;

		if ( ImGui::Begin("test window") )
		{
			if ( ::Tridium::Refl::MetaRegistry::TryGetMetaPropertyFromClass<Derived>( drawFunc, ::Tridium::Editor::Internal::DrawPropFuncID ) )
			{
				drawFunc( "derivedInstance", derivedInstance, static_cast<::Tridium::Refl::PropertyFlags>(::Tridium::Refl::EPropertyFlag::EditAnywhere));
			}

		}
		ImGui::End();
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