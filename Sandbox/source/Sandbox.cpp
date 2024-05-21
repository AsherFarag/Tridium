#include <Tridium.h>

#include "ImGui/imgui.h"

// TEMP
#define SOL_ALL_SAFETIES_ON 1
#include <sol2/include/sol/sol.hpp>
#include <Tridium/Scripting/Script.h>
#include <fstream>
using namespace std;

class ExampleLayer : public Tridium::Layer
{
public:
	ExampleLayer()
		: Layer( "Example" ) {}

	void OnUpdate() override
	{
	}

	//sol::state lua;
	ofstream luaFileO;
	ifstream luaFileI;
	std::string luaSrc;

	virtual void OnAttach() override
	{
		// open some common libraries
		//lua.open_libraries( sol::lib::base, sol::lib::math, sol::lib::package );

		luaSrc.reserve( 1024 * 16 );

		luaFileI.open( "Content/Scripts/Testlua.lua" );
		if (luaFileI)
		{
			ostringstream ss;
			ss << luaFileI.rdbuf();
			luaSrc = ss.str();
		}
		luaFileI.close();
	}

	virtual void OnImGuiDraw() override
	{
		ImGui::Begin( "Lua");

		ImGui::InputTextMultiline( "Source", luaSrc.data(), 1024 * 16 * sizeof(char));

		if ( ImGui::Button( "Save" ) )
		{
			std::ofstream ofs;
			ofs.open( "Content/Scripts/Testlua.lua", std::ofstream::out | std::ofstream::trunc );
			ofs.close();

			luaFileO.open( "Content/Scripts/Testlua.lua" );
			luaFileO << luaSrc;
			luaFileO.close();
		}

		if ( ImGui::Button( "Recompile Lua Scripts" ) )
		{
			Tridium::ScriptLibrary::Get( "Testlua" )->Recompile();
		}


		ImGui::End();
	}

	void OnEvent( Tridium::Event& event ) override
	{
		//LOG_INFO( "{0}", event.ToString() );
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