#include <Tridium.h>

#include "ImGui/imgui.h"

// TEMP
#define SOL_ALL_SAFETIES_ON 1
#include <sol2/include/sol/sol.hpp>

class ExampleLayer : public Tridium::Layer
{
public:
	ExampleLayer()
		: Layer( "Example" ) {}

	void OnUpdate() override
	{
		//LOG_INFO( "ExampleLayer:Update" );
	}

	sol::state lua;

	virtual void OnAttach() override
	{
		// open some common libraries
		lua.open_libraries( sol::lib::base, sol::lib::math, sol::lib::package );
	}

	virtual void OnImGuiDraw() override
	{
		ImGui::Begin( "Lua" );
		if ( ImGui::Button( "Recompile Lua Scripts" ) )
		{
			lua.do_file( "Testlua.lua", sol::load_mode::text );
		}
		ImGui::End();
	}

	void OnEvent( Tridium::Event& event ) override
	{
		LOG_INFO( "{0}", event.ToString() );
	}
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		PushLayer( new ExampleLayer() );
	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}