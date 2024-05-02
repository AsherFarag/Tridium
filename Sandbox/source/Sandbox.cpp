#include <Tridium.h>

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}