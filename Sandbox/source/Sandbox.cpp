#include <Tridium.h>

#include "ImGui/imgui.h"

class ExampleLayer : public Tridium::Layer
{
public:
	ExampleLayer()
		: Layer( "Example" ) {}

	void OnUpdate() override
	{
		//LOG_INFO( "ExampleLayer:Update" );
	}

	virtual void OnImGuiDraw() override
	{
		ImGui::Begin( "Test" );
		ImGui::Text( "Hi!" );
		ImGui::End();
	}

	void OnEvent( Tridium::Event& event ) override
	{
		LOG_WARN( "{0}", event.ToString() );
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