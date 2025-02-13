#include "tripch.h"
#include "EditorDocumentation.h"

namespace Tridium {

	void RegisterGameObjectDocumentation( EditorDocumentation& a_Storage );
	void RegisterComponentDocumentation( EditorDocumentation& a_Storage );

	void EditorDocumentation::Init()
	{
		RegisterComponentDocumentation( *this );
	}

	void RegisterGameObjectDocumentation( EditorDocumentation& a_Storage )
	{
		TypeDocumentation doc;
		doc.Name = "GameObject";
		doc.Description = "A GameObject is the base entity in the ECS system. It is a container for components and can be parented to other GameObjects.";
		doc.Members = { };
		doc.Functions = { };
		a_Storage.RegisterType( doc );
	}

	void RegisterComponentDocumentation( EditorDocumentation& a_Storage )
	{
		// Native Script Components
		{
			// NativeScriptComponent
			{
				TypeDocumentation doc;
				doc.Name = "NativeScriptComponent";
				doc.Description = "A component that allows you to write scripting functionality and attach it to a GameObject.";
				doc.Members = { };
				doc.Functions = { };
				a_Storage.RegisterType( doc );
			}

			// TransformComponent
			{
				TypeDocumentation doc;
				doc.Name = "TransformComponent";
				doc.Description =
R"(A component that represents the position, rotation, and scale of a GameObject in 3D space.
It is also in charge of parenting and child relationships between GameObjects.
)";
				doc.Members = { };
				doc.Functions = { };
				a_Storage.RegisterType( doc );
			}

			// StaticMeshComponent
			{
				TypeDocumentation doc;
				doc.Name = "StaticMeshComponent";
				doc.Description = "A component that represents a static mesh that can be rendered in the scene.";
				doc.Members = { };
				doc.Functions = { };
				a_Storage.RegisterType( doc );
			}
		}
	}

}
