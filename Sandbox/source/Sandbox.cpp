#include <Tridium.h>
#include <Tridium/ECS/Components/Types/Common/TestComponent.h>

struct PersonInfo
{
	REFLECT(PersonInfo);

	~PersonInfo()
	{
		LOG_INFO( "PersonInfo Destructor" );
	}

	int Age = 0;
	std::string Name = "John Doe";
	float Height = 0.0f;
};

BEGIN_REFLECT( PersonInfo )
	PROPERTY( Age, FLAGS( EditAnywhere ) )
	PROPERTY( Name, FLAGS( EditAnywhere ) )
	PROPERTY( Height, FLAGS( EditAnywhere ) )
END_REFLECT( PersonInfo )

class Person : public Tridium::Component
{
	REFLECT(Person);
public:
	PersonInfo Info;
	std::map<std::string, PersonInfo> Children;
	std::vector<int> Numbers = { 1, 2, 3 };
};

BEGIN_REFLECT_COMPONENT( Person )
	PROPERTY( Info, FLAGS( EditAnywhere ) )
	PROPERTY( Children, FLAGS( EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( EditAnywhere ) )
END_REFLECT( Person )

class ExampleLayer : public Tridium::Layer
{

public:
	int m = 5;
	ExampleLayer() = default;

	virtual void OnImGuiDraw() override
	{
		//Tridium::s_ReflectorTestComponent;
		auto t = entt::resolve<Tridium::TestComponent>();
		LOG_INFO( "{0}", Tridium::Refl::MetaRegistry::GetCleanTypeName(t) );

		t = entt::resolve( t.id() );
		LOG_INFO( "{0}", t.info().name() );
	}
};