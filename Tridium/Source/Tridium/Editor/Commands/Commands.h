#pragma once
#include <Tridium/ECS/GameObject.h>

namespace Tridium {

	namespace Commands {

		struct GameObjectCreated
		{
			void Undo()
			{
				if ( Scene.expired() )
					return;

				Scene.lock()->DestroyGameObject( GameObject );
			}

			void Redo()
			{
				if ( Scene.expired() )
					return;
				GameObject = Scene.lock()->InstantiateGameObject();
			}

			WeakPtr<Scene> Scene;
			GameObject GameObject;
			Array<Refl::MetaAny> Components;
		};

		struct GameObjectDestroyed
		{
			void Undo()
			{
				if ( Scene.expired() )
					return;

				GameObject = Scene.lock()->InstantiateGameObject();
			}
			void Redo()
			{
				if ( Scene.expired() )
					return;

				Scene.lock()->DestroyGameObject( GameObject );
			}

			WeakPtr<Scene> Scene;
			GameObject GameObject;
		};


	} // namespace Commands

} // namespace Tridium