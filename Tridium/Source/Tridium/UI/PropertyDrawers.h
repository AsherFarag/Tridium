#pragma once
#include <Tridium/UI/UI.h>

#if CONFIG_ENABLE_TOOL_UI

#include <Tridium/Reflection/MetaAttributes.h>

namespace Tridium {

	template<Meta::IsMeta T>
	struct UIMetaDrawer
	{
		template<typename _Instance>
		static bool Draw( StringView a_DefaultLabel, _Instance& a_Instance )
		{
			return false;
		}
	};

	template<typename T>
	struct UIPropertyDrawer
	{
		static bool Draw( StringView a_Label, T& a_Value, bool a_CanWrapInTreeNode = true )
		{
			if constexpr ( Meta::HasReflector<T> )
			{
				// If there is a user-defined reflector, use it
				return UIPropertyDrawer<Meta::Reflector<T>>::Draw( a_Label, a_Value, a_CanWrapInTreeNode );
			}
			else if constexpr ( Concepts::AggregateReflectable<T> )
			{
				bool modified = false;

				if ( a_CanWrapInTreeNode )
					if ( !UI::BeginTree( a_Label ) )
						return false; // Early out if the tree node is not open

				UI::BeginPropertyGrid();

				ForEachField( a_Value, [&]( StringView a_FieldName, auto& a_Field )
				{
					using Drawer = UIPropertyDrawer<std::decay_t<decltype( a_Field )>>;
					modified |= Drawer::Draw( a_FieldName, a_Field );
				} );

				UI::EndPropertyGrid();

				if ( a_CanWrapInTreeNode )
					UI::EndTree();

				return modified;
			}
			else
			{
				// No drawer available, do nothing
				return false;
			}
		}
	};

	template<typename T>
	struct UIPropertyDrawer<Meta::Reflector<T>>
	{
		static bool Draw( StringView a_Label, T& a_Value, bool a_CanWrapInTreeNode = true )
		{
			const auto DrawFunc = [&]()
			{
				bool modified = false;

				Meta::Reflector<T> reflector{};

				if ( UI::IsPropertyGridOpen() )
				{
					if constexpr ( Meta::GetType<T>().template Has<Meta::DisplayName>() )
					{
						a_Label = Meta::GetType<T>().template Get<Meta::DisplayName>().Value;
					}
					else
					{
						a_Label = GetStrippedTypeName<T>();
					}
				}

				if ( a_CanWrapInTreeNode )
					if ( !UI::BeginTree( a_Label ) )
						return false; // Early out if the tree node is not open

				UI::BeginPropertyGrid();

				ForEachField( reflector, [&]( StringView a_FieldName, auto& a_Field )
				{
					modified |= UIMetaDrawer<std::decay_t<decltype( a_Field )>>::Draw( a_FieldName, a_Value );
				} );

				UI::EndPropertyGrid();

				if ( a_CanWrapInTreeNode )
					UI::EndTree();

				return modified;
			};

			if ( UI::IsPropertyGridOpen() )
			{
				return UI::DrawGridProperty( a_Label, DrawFunc );
			}
			else
			{
				return DrawFunc();
			}
		}
	};

#pragma region Math Drawers

	template<size_t N, typename T>
	struct UIPropertyDrawer<Vector<N, T>>
	{
		static bool Draw( StringView a_Label, Vector<N, T>& a_Vector,
						  const T* a_Min = nullptr, const T* a_Max = nullptr )
		{
			return UI::DrawVector( a_Label, a_Vector,
								   a_Min,
								   a_Max );
		}
	};

#pragma endregion

#pragma region Meta Drawers

	template<auto _Accessor, typename... _Attributes>
	struct UIMetaDrawer<Meta::Field<_Accessor, _Attributes...>>
	{
		using FieldType = typename Meta::Field<_Accessor, _Attributes...>;

		template<typename _Instance>
		static bool Draw( StringView a_DefaultLabel, _Instance& a_Instance )
		{
			if constexpr ( FieldType::template Has<Meta::DisplayIf>() )
			{
				if ( !FieldType::template Get<Meta::DisplayIf>().Evaluate( a_Instance ) )
				{
					return false;
				}
			}

			if constexpr ( FieldType::template Has<Meta::DisplayName>() )
			{
				a_DefaultLabel = FieldType::template Get<Meta::DisplayName>().Value;
			}

			if constexpr ( FieldType::template Has<Meta::Editable>() )
			{
				auto value = FieldType::GetValue( a_Instance );
				if ( UIPropertyDrawer<std::decay_t<decltype( value )>>::Draw( a_DefaultLabel, value ) )
				{
					FieldType::SetValue( a_Instance, value );
					return true;
				}

				return false;
			}
			else
			{
				// Not editable
				return false;
			}
		}
	};

	template<auto _Function, typename... _Attributes>
	struct UIMetaDrawer<Meta::Function<_Function, _Attributes...>>
	{
		using FunctionType = typename Meta::Function<_Function, _Attributes...>;

		template<typename _Instance>
		static bool Draw( StringView a_DefaultLabel, _Instance& a_Instance )
		{
			if constexpr ( !FunctionType::template Has<Meta::CallInEditor>() )
			{
				return false;
			}
			else
			{
				if constexpr ( FunctionType::template Has<Meta::DisplayIf>() )
				{
					if ( !FunctionType::template Get<Meta::DisplayIf>().Evaluate( a_Instance ) )
					{
						return false;
					}
				}

				if constexpr ( FunctionType::template Has<Meta::DisplayName>() )
				{
					a_DefaultLabel = FunctionType::template Get<Meta::DisplayName>().Value;
				}

				UI::EndPropertyGrid();

				bool invoked = ImGui::Button( a_DefaultLabel.data() );

				UI::BeginPropertyGrid();

				if ( invoked )
				{
					if constexpr ( FunctionType::IsStatic )
					{
						FunctionType{}( );
					}
					else
					{
						FunctionType{}( a_Instance );
					}

					return true;
				}

				return false;
			}
		}
	};

	template<>
	struct UIMetaDrawer<Meta::Header>
	{
		template<typename _Instance>
		static bool Draw( StringView a_DefaultLabel, _Instance& a_Instance )
		{
			UI::EndPropertyGrid();
			UI::SeparatorText( a_DefaultLabel );
			UI::BeginPropertyGrid();

			return false;
		}
	};

#pragma endregion


} // namespace Tridium

#endif // CONFIG_ENABLE_TOOL_UI