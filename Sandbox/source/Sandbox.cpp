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

	virtual void OnAttach() override
	{
	}

	virtual void OnImGuiDraw() override
	{

	}
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		PushOverlay( new ExampleLayer() );
	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}