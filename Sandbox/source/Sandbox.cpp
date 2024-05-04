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
		auto s = event.ToString();
		LOG_INFO( "{0}", s.c_str() );
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