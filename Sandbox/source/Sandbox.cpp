#include <Tridium.h>

class ExampleLayer : public Tridium::Layer
{
public:
	ExampleLayer()
		: Layer( "Example" ) {}

	void OnUpdate() override
	{
		//LOG_INFO( "ExampleLayer:Update" );
	}

	void OnEvent( Tridium::Event& event ) override
	{
		//LOG_INFO( "{0}", event.GetName() );
	}
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		PushLayer( new Tridium::ImGuiLayer() );
	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}