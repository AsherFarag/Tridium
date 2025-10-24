#include "InspectorPanel.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/UI/PropertyDrawers.h>

namespace Tridium {

	struct MyCustomComponent
	{
		String Name = "Player";
		String Description = "This is the player component.";
		float Health = 100.0f;
		static constexpr float MaxHealth = 100.0f;
		float HealthRegenRate = 5.0f;
		float TimeDead = 0.0f;
		int32_t Damage = 10;
		float AttackSpeed = 1.0f;
		Vector3 WeaponOffset{};
		Vector3 Color{ 0.0f, 0.0f, 1.0f };

		bool IsDead() const 
		{ 
			return Health <= 0.0f; 
		}

		void Attack() 
		{ 
			LOG( LogCategory::GameLogic, Debug, "Player attacked for {} damage!", Damage ); 
		}
	};

	namespace Meta {

		template<>
		struct Reflector<MyCustomComponent>
		{
			// This declares custom type attributes the reflection system can use.
			using Type = Type<MyCustomComponent, DisplayName<"Player Component">, Icon<EditorIcons::Person>>;

			// This will display a seperator in the editor called "General"
			Header General;
			
			Field<&MyCustomComponent::Name, Editable, Serializable>
			Name;

			Field<&MyCustomComponent::Description, Editable, Serializable, MultilineText>
			Description;

			Header HealthStats;

			Field<&MyCustomComponent::Health, Editable, Serializable, Range<0.0f, MyCustomComponent::MaxHealth>>
			Health;

			Field<&MyCustomComponent::HealthRegenRate, Editable, Serializable>
			HealthRegenRate;

			// Meta members can be hidden using the DisplayIf attribute.
			// This will only display the attribute if the function returns true.
			Field<&MyCustomComponent::TimeDead, Visible,
			DisplayIf<&MyCustomComponent::IsDead>,
			Tooltip<"I'm only shown if the player is dead.">>
			TimeDead;

			Header CombatStats;

			Field<&MyCustomComponent::Damage, Editable, Serializable, Min<0>>
			Damage;

			Field<&MyCustomComponent::AttackSpeed, Editable, Serializable, Min<0.1f>>
			AttackSpeed;

			Field<&MyCustomComponent::WeaponOffset, Editable, Serializable,
			Tooltip<"The offset position of the weapon relative to the player.">>
			WeaponOffset;

			Header Misc;

			// Properties also allow free functions!
			// I'm able to construct a lambda and pass it in as a getter. 
			Property<[]( MyCustomComponent& c ){ return !c.IsDead(); }, nullptr, Visible,
			DisplayName<"Am I alive?">>
			IsAlive;

			// Functions are also reflectable.
			// With the CallInEditor attribute, this function can show up in the editor and be invocable.
			// (Only works on functions with no arguments.)
			Function<&MyCustomComponent::Attack, CallInEditor,
			DisplayName<"Do an Attack!">,
			Tooltip<"Makes the player perform an attack action.">>
			Attack;
		};
	}

	void InspectorPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		if ( ImGui::Begin( a_Name.data(), &o_Open ) )
		{
			UI_DrawComponents();
		}

		ImGui::End();
	}

	void InspectorPanel::UI_DrawComponents()
	{
		static TransformComponent transform{};

		if ( UI::BeginTree( "Transform", UI::ETreeFlags::DefaultOpen | UI::ETreeFlags::Framed ) )
		{
			UIPropertyDrawer<TransformComponent>::Draw( "Transform", transform, false );
			UI::EndTree();
		}

		static MyCustomComponent player{};
		static String playerComponentName = std::format( "{} Player Component", StringView( EditorIcons::Person ), player.Name );

		if ( UI::BeginTree( playerComponentName, UI::ETreeFlags::DefaultOpen | UI::ETreeFlags::Framed ) )
		{
			UIPropertyDrawer<MyCustomComponent>::Draw( playerComponentName, player, false /* Don't draw a tree node */ );
			UI::EndTree();
		}

		if ( player.IsDead() )
		{
			player.TimeDead += 0.016f; // Simulate time dead increment
		}
		else
		{
			player.TimeDead = 0.0f; // Reset time dead if alive
		}
	}

} // namespace Tridium

#endif // WITH_EDITOR