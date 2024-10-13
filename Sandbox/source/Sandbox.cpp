#include <Tridium.h>
#include <Tridium/IO/SceneSerializer.h>

struct PersonInfo
{
	int Age = 0;
	std::string Name = "John Doe";
	float Height = 0.0f;
	bool IsMale = true;

	std::vector<std::string> Numbers = { "1", "2", "3" };
	std::map<std::string, std::string> Map = { { "Key1", "Value1" }, { "Key2", "Value2" } };
};

BEGIN_REFLECT( PersonInfo )
	PROPERTY( Age, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Name, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Height, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( IsMale, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Map, FLAGS( Serialize, EditAnywhere ) )
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
	PROPERTY( Info, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Children, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( Person )

class TestLayer : public Tridium::Layer
{
	virtual void OnImGuiDraw() override
	{
	}
};

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
	}
};

Tridium::GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}