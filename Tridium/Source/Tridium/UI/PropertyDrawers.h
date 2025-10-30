#pragma once
#include <Tridium/UI/UI.h>

#if CONFIG_ENABLE_TOOL_UI

#include <Tridium/Reflection/MetaAttributes.h>
#include <Tridium/Asset/AssetDatabase.h>

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

	template<typename T, Meta::IsAttributeList _Attributes = Meta::AttributeList<>>
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
				const auto DrawFunc = [&]()
				{
					bool modified = false;

					StringView treeLabel = UI::IsPropertyGridOpen() ? a_Label : GetStrippedTypeName<T>();

					if ( a_CanWrapInTreeNode )
					{
						// In case we are not editable, we still want the tree node to be shown
						UI::ScopedOverrideEnable enableTreeNode{};
						if ( !UI::BeginTree( treeLabel ) )
							return false; // Early out if the tree node is not open
					}

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

				StringView treeLabel = a_Label;

				if ( UI::IsPropertyGridOpen() )
				{
					if constexpr ( Meta::GetType<T>().template Has<Meta::DisplayName>() )
					{
						treeLabel = Meta::GetType<T>().template Get<Meta::DisplayName>().Value;
					}
					else
					{
						treeLabel = GetStrippedTypeName<T>();
					}
				}

				if ( a_CanWrapInTreeNode )
				{
						// In case we are not editable, we still want the tree node to be shown
					UI::ScopedOverrideEnable enableTreeNode{};
					if ( !UI::BeginTree( treeLabel ) )
						return false; // Early out if the tree node is not open
				}

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

	template<typename T> requires std::is_enum_v<T>
	struct UIPropertyDrawer<Meta::Reflector<T>>
	{
		static bool Draw( StringView a_Label, T& a_Enum, bool a_CanWrapInTreeNode = true )
		{
			const auto DrawFunc = [&]() -> bool
			{
				Meta::Reflector<T> reflector{};
				bool modified = false;
				StringView previewValue{};

				// Get preview value
				ForEachField( reflector, [&]( StringView a_FieldName, auto& a_Field )
				{
					using ConstantType = std::decay_t<decltype( a_Field )>;
					if constexpr ( Meta::IsConstant<ConstantType> )
					{
						if constexpr ( a_Field.template Has<Meta::DisplayName>() )
						{
							a_FieldName = a_Field.template Get<Meta::DisplayName>().Value;
						}

						const T enumValue = static_cast<T>( a_Field.Value );
						const bool isSelected = ( a_Enum == enumValue );

						if ( isSelected )
						{
							previewValue = a_FieldName;
						}
					}
				} );

				const bool open = ImGui::BeginCombo( UI::IsPropertyGridOpen() ? a_Label.data() : UI::GenerateID(),
												   previewValue.data(),
												   ImGuiComboFlags_None );

				if ( open )
				{
					ForEachField( reflector, [&]( StringView a_FieldName, auto& a_Field )
					{
						using ConstantType = std::decay_t<decltype( a_Field )>;
						if constexpr ( Meta::IsConstant<ConstantType> )
						{
							if constexpr ( a_Field.template Has<Meta::DisplayName>() )
							{
								a_FieldName = a_Field.template Get<Meta::DisplayName>().Value;
							}

							const T enumValue = static_cast<T>( a_Field.Value );
							const bool isSelected = ( a_Enum == enumValue );

							if ( ImGui::Selectable( a_FieldName.data(), isSelected ) )
							{
								a_Enum = enumValue;
								modified = true;
							}

							if ( isSelected )
							{
								ImGui::SetItemDefaultFocus();
							}
						}
					} );

					ImGui::EndCombo();
				}

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

#pragma region Drawers

	template<Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<bool, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, bool& a_Bool )
		{
			return UI::DrawCheckbox( a_Label, a_Bool );
		}
	};

	template<Concepts::Arithmetic T, Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<T, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, T& a_Value )
		{
			if constexpr ( Attributes::template Has<Meta::Range>() )
			{
				constexpr auto rangeAttr = Attributes::template Get<Meta::Range>();
				const T min = Cast<float>( rangeAttr.Min );
				const T max = Cast<T>( rangeAttr.Max );
				bool modified = UI::DrawScalar( a_Label, a_Value,
									   rangeAttr.HasMin ? &min : nullptr,
									   rangeAttr.HasMax ? &max : nullptr );

				if constexpr ( rangeAttr.HasMax ) a_Value = Math::Min( a_Value, max );
				if constexpr ( rangeAttr.HasMin ) a_Value = Math::Max( a_Value, min );

				return modified;
			}
			else
			{
				return UI::DrawScalar( a_Label, a_Value );
			}
		}
	};

	template<size_t N, typename _GenType, Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<Vector<N, _GenType>, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, Vector<N, _GenType>& a_Vector )
		{
			if constexpr ( Attributes::template Has<Meta::Range>() )
			{
				constexpr auto rangeAttr = Attributes::template Get<Meta::Range>();
				const _GenType min = Cast<_GenType>( rangeAttr.Min );
				const _GenType max = Cast<_GenType>( rangeAttr.Max );
				bool modified = UI::DrawVector( a_Label, a_Vector,
									   rangeAttr.HasMin ? &min : nullptr,
									   rangeAttr.HasMax ? &max : nullptr );

				if constexpr ( rangeAttr.HasMax ) a_Vector = Math::Min( a_Vector, Vector<N, _GenType>( max ) );
				if constexpr ( rangeAttr.HasMin ) a_Vector = Math::Max( a_Vector, Vector<N, _GenType>( min ) );

				return modified;
			}
			else
			{
				return UI::DrawVector( a_Label, a_Vector );
			}
		}
	};

	template<Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<String, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, String& a_String )
		{
			if constexpr ( Attributes::template Has<Meta::MultilineText>() )
			{
				return UI::DrawInputTextMultiline( a_Label, a_String );
			}
			else
			{
				return UI::DrawInputText( a_Label, a_String );
			}
		}
	};

	template<Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<Color3, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, Color3& a_Color )
		{
			if ( UI::IsPropertyGridOpen() )
			{
				return UI::DrawGridProperty( a_Label, [&]() 
				{
					bool modified = ImGui::ColorEdit3( UI::GenerateID(), reinterpret_cast<float*>( &a_Color ), ImGuiColorEditFlags_None );
					return modified;
				} );
			}
			else
			{
				bool modified = ImGui::ColorEdit3( a_Label.data(), reinterpret_cast<float*>( &a_Color ), ImGuiColorEditFlags_None );
				return modified;
			}

			return false;
		}
	};

	template<Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<Color4, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, Color4& a_Color )
		{
			if ( UI::IsPropertyGridOpen() )
			{
				return UI::DrawGridProperty( a_Label, [&]()
				{
					bool modified = ImGui::ColorEdit4( UI::GenerateID(), reinterpret_cast<float*>( &a_Color ), ImGuiColorEditFlags_None );
					return modified;
				} );
			}
			else
			{
				bool modified = ImGui::ColorEdit4( a_Label.data(), reinterpret_cast<float*>( &a_Color ), ImGuiColorEditFlags_None );
				return modified;
			}

			return false;
		}
	};

	template<Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<UUID, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, UUID& a_UUID )
		{
			if ( UI::IsPropertyGridOpen() )
			{
				UI::DrawGridProperty( a_Label, [&]() 
				{
					ImGui::TextUnformatted( std::format( "{}", a_UUID ).c_str() );
					return false;
				} );
			}
			else
			{
				ImGui::TextUnformatted( std::format( "{}: {}", a_Label, a_UUID ).c_str() );
			}

			return false;
		}
	};

	template<typename _Type, Meta::IsAttributeList _Attributes>
	struct UIPropertyDrawer<AssetHandle<_Type>, _Attributes>
	{
		using Attributes = _Attributes;
		static bool Draw( StringView a_Label, AssetHandle<_Type>& a_AssetHandle )
		{
			const auto DrawFunc = [&]( StringView a_Label ) -> bool
			{
				bool modified = false;

				StringView assetName = "None";
				if ( const AssetInfo* assetInfo = a_AssetHandle.Info(); assetInfo && !assetInfo->Name.empty() )
				{
					assetName = assetInfo->Name;
				}

				const bool open = ImGui::BeginCombo( a_Label.data(), assetName.data() );

				if ( open )
				{
					if ( ImGui::Selectable( "None###Internal", !a_AssetHandle.Valid() ) )
					{
						a_AssetHandle = AssetHandle<_Type>{};
						ImGui::EndCombo();
						return true;
					}

					ImGui::Separator();

					int idx = 0;
					AssetDatabase::ForEachAssetOfType<_Type>( [&]( const AssetInfo& a_AssetInfo, const _Type* a_Asset )
					{
						StringView name = !a_AssetInfo.Name.empty() ? a_AssetInfo.Name : a_AssetInfo.Path;

						ImGui::ScopedID id( idx++ );

						bool selected = a_AssetHandle.ID() == a_AssetInfo.ID;
						if ( ImGui::Selectable( name.data(), selected ) && !selected )
						{
							a_AssetHandle = AssetHandle<_Type>( a_AssetInfo.ID );
							modified = true;
						}
					} );

					ImGui::EndCombo();
				}

				return modified;
			};

			if ( UI::IsPropertyGridOpen() )
			{
				return UI::DrawGridProperty( a_Label, [&]() { return DrawFunc( UI::GenerateID() ); } );
			}
			else
			{
				return DrawFunc( a_Label );
			}
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

			bool modified = false;

			if constexpr ( FieldType::template Has<Meta::Editable>() )
			{
				ImGui::BeginGroup(); 
				auto value = FieldType::GetValue( a_Instance );
				using Drawer = UIPropertyDrawer<std::decay_t<decltype( value )>, Meta::AttributeList<_Attributes...>>;

				if ( Drawer::Draw( a_DefaultLabel, value ) )
				{
					FieldType::SetValue( a_Instance, value );
					modified = true;
				}
				ImGui::EndGroup();
			}
			else if constexpr ( FieldType::template Has<Meta::Visible>() )
			{
				ImGui::BeginGroup();
				UI::BeginDisabled();

				auto value = FieldType::GetValue( a_Instance );
				using Drawer = UIPropertyDrawer<std::decay_t<decltype( value )>, Meta::AttributeList<_Attributes...>>;
				Drawer::Draw( a_DefaultLabel, value );

				UI::EndDisabled();
				ImGui::EndGroup();
			}
			else
			{
				// Not editable
				return false;
			}

			if constexpr ( FieldType::template Has<Meta::Tooltip>() )
			{
				if ( ImGui::BeginItemTooltip() )
				{
					constexpr StringView tooltip = FieldType::template Get<Meta::Tooltip>().Value;
					ImGui::TextUnformatted( tooltip.data(), tooltip.data() + tooltip.size() );
					ImGui::EndTooltip();
				}
			}

			return modified;
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

				if constexpr ( FunctionType::template Has<Meta::Tooltip>() )
				{
					if ( ImGui::BeginItemTooltip() )
					{
						constexpr StringView tooltip = FunctionType::template Get<Meta::Tooltip>().Value;
						ImGui::TextUnformatted( tooltip.data(), tooltip.data() + tooltip.size() );
						ImGui::EndTooltip();
					}
				}

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