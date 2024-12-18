#include "tripch.h"
#ifdef IS_EDITOR

#include "EditorReflection.h"
#include <Tridium/ImGui/IconsFontAwesome6.h>

namespace Tridium {

	namespace Refl::Internal {
        using enum ::Tridium::Refl::EPropertyFlag;
        using ::Tridium::Editor::Internal::DrawPropFuncID;
        using ::Tridium::Editor::Internal::DrawPropFunc;

#pragma region Helpers

        struct ElementReturn
        {
			uint8_t IsOpen : 1;
			uint8_t WasRemoved : 1;
        };

        bool DrawRemoveElementButton( const char* a_Name )
        {
            ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ( ImGui::CalcTextSize( a_Name ).x + 10 ) );
			ImGui::ScopedStyleCol col( ImGuiCol_Button, Editor::Style::Colors::Red.Value );
            return ImGui::SmallButton( a_Name );
        }

        ElementReturn BeginElementTreeNode( const char* a_Name, PropertyFlags a_Flags )
        {
            ElementReturn ret;

            ret.IsOpen = ImGui::TreeNodeEx( a_Name, ImGuiTreeNodeFlags_DefaultOpen );

            if ( HasFlag( a_Flags, EPropertyFlag::EditAnywhere ) )
            {
                ImGui::SameLine();
                ret.WasRemoved = DrawRemoveElementButton( TE_ICON_TRASH_CAN );
            }

            return ret;
        }

        bool DrawAddElementButton( const char* a_Name )
        {
			ImGui::Separator();
            ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x * 0.5f - ( ImGui::CalcTextSize( a_Name ).x + 10 ) );
            return ImGui::SmallButton( a_Name );
        }

#pragma endregion

        bool DrawKeyToValueAssociativeContainer( entt::meta_associative_container& a_AssociativeContainer, const MetaData& a_MetaData, PropertyFlags a_DrawFlag )
        {
            if ( !ImGui::TreeNodeEx( a_MetaData.name().c_str() ) )
            {
                if ( ImGui::BeginItemTooltip() )
                {
                    ImGui::Text( "Class: Map Container of '%s' to '%s'",
                        MetaRegistry::GetCleanTypeName( a_AssociativeContainer.key_type() ),
                        MetaRegistry::GetCleanTypeName( a_AssociativeContainer.mapped_type() ) );
                    ImGui::EndTooltip();
                }
                return false;
            }

            if ( ImGui::BeginItemTooltip() )
            {
                ImGui::Text( "Class: Map Container of '%s' to '%s'",
                    MetaRegistry::GetCleanTypeName( a_AssociativeContainer.key_type() ),
                    MetaRegistry::GetCleanTypeName( a_AssociativeContainer.mapped_type() ) );
                ImGui::EndTooltip();
            }

            ImGui::FunctionScope treePop( +[]() { ImGui::TreePop(); } );

            bool wasChanged = false;

            auto keyDrawFuncProp = a_AssociativeContainer.key_type().prop( DrawPropFuncID );
            auto valueDrawFuncProp = a_AssociativeContainer.mapped_type().prop( DrawPropFuncID );

            if ( !keyDrawFuncProp || !valueDrawFuncProp )
                return false;

            auto keyDrawFunc = keyDrawFuncProp.value().cast<DrawPropFunc>();
            auto valueDrawFunc = valueDrawFuncProp.value().cast<DrawPropFunc>();


            // Iterate through each element of the map
            size_t index = 0;
            for ( auto&& [key, value] : a_AssociativeContainer )
            {
                const std::string elementName = std::string( "Element " ) + std::to_string( index );
                ++index;
                ElementReturn treeNodeResult = BeginElementTreeNode( elementName.c_str(), a_DrawFlag );

                if ( treeNodeResult.WasRemoved )
                {
                    a_AssociativeContainer.erase( key );
                    wasChanged = true;

                    if ( treeNodeResult.IsOpen )
                        ImGui::TreePop();

                    break;
                }

                if ( !treeNodeResult.IsOpen )
                    continue;

                // Create a temporary so that, if changed, we can replace the original
                auto tempKey = key;

                ImGui::PushItemWidth( ImGui::GetContentRegionAvail().x - 100 );
                wasChanged |= keyDrawFunc( "Key", tempKey, a_DrawFlag);
                wasChanged |= valueDrawFunc( "Value", value, a_DrawFlag );
                ImGui::PopItemWidth();

                ImGui::TreePop();

                if ( wasChanged )
                {
                    auto tempVal = value;
                    a_AssociativeContainer.erase( key );
                    a_AssociativeContainer.insert( std::move( tempKey ), std::move( tempVal ) );
                    break;
                }
            }

            if ( !HasFlag(a_DrawFlag, EPropertyFlag::EditAnywhere) 
                && a_AssociativeContainer.size() == 0 )
            {
                ImGui::Text( "Empty" );
            }

            if ( HasFlag( a_DrawFlag, EPropertyFlag::EditAnywhere )
                && DrawAddElementButton( "Add Element" ) )
            {
                auto newKey = a_AssociativeContainer.key_type().construct();
                if ( a_AssociativeContainer.size() == 0 
                    || a_AssociativeContainer.find( newKey ) == a_AssociativeContainer.end() )
                {
                    a_AssociativeContainer.insert(
                        std::move( newKey ),
                        std::move( a_AssociativeContainer.mapped_type().construct() ) );
                    wasChanged = true;
                }
            }

            return wasChanged;
        }

        bool DrawSequenceContainer( entt::meta_sequence_container& a_SequenceContainer, const MetaData& a_MetaData, PropertyFlags a_DrawFlag )
        {
            if ( !ImGui::TreeNodeEx( a_MetaData.name().c_str() ) )
            {
                if ( ImGui::BeginItemTooltip() )
                {
                    ImGui::Text( "Class: Sequence Container of '%s'", MetaRegistry::GetCleanTypeName( a_SequenceContainer.value_type() ) );
                    ImGui::EndTooltip();
                }
                return false;
            }

            if ( ImGui::BeginItemTooltip() )
            {
                ImGui::Text( "Class: Sequence Container of '%s'", MetaRegistry::GetCleanTypeName( a_SequenceContainer.value_type() ) );
                ImGui::EndTooltip();
            }

            bool wasChanged = false;

            if ( auto drawFuncProp = a_SequenceContainer.value_type().prop( DrawPropFuncID ) )
            {
                auto drawFunc = drawFuncProp.value().cast<DrawPropFunc>();

                size_t index = 0;
                for ( auto it = a_SequenceContainer.begin(); it != a_SequenceContainer.end(); ++it )
                {
					MetaAny elementRef = *it;

                    const std::string elementName = std::to_string( index );

                    float oldCursorPosY = ImGui::GetCursorPosY();
					ImGui::PushItemWidth( ImGui::GetContentRegionAvail().x - 100 );
                    wasChanged |= drawFunc( elementName.c_str(), elementRef, a_DrawFlag );
					ImGui::PopItemWidth();
					ImVec2 currentCursorPos = ImGui::GetCursorPos();

					if ( HasFlag( a_DrawFlag, EPropertyFlag::EditAnywhere ) )
                    {
						ImGui::ScopedID elementID( index );
						ImGui::SetCursorPosY( oldCursorPosY + 2.5f );
                        if ( DrawRemoveElementButton( TE_ICON_TRASH_CAN ) )
                        {
                            a_SequenceContainer.erase( it );
                            wasChanged = true;
                            break;
                        }

                        ImGui::SetCursorPos( currentCursorPos );
                    }

                    ++index;
                }
            }

			if ( HasFlag( a_DrawFlag, EPropertyFlag::EditAnywhere ) )
            {
                if ( DrawAddElementButton( "Add Element" ) )
                {
                    auto newElement = MetaRegistry::ResolveMetaType( a_SequenceContainer.value_type().info() ).construct();
                    a_SequenceContainer.insert( a_SequenceContainer.end(), std::move(newElement) );
                    wasChanged = true;
                }
            }

			if ( !HasFlag( a_DrawFlag, EPropertyFlag::EditAnywhere ) 
                && a_SequenceContainer.size() == 0 )
			{
				ImGui::Text( "Empty" );
			}

            ImGui::TreePop();

            return wasChanged;
        }

		bool DrawAllMembersOfMetaClass( const MetaType& a_MetaType, MetaAny& a_Handle, EPropertyFlag a_OverrideFlag )
        {
            bool wasChanged = false;

            for ( auto&& [id, metaData] : a_MetaType.data() )
            {
                bool memberWasChanged = false;
                PropertyFlags drawFlag = static_cast<PropertyFlags>( EPropertyFlag::None );

                // Get the appropiate draw flag
                // If the draw flag is none, skip the property
                if ( HasFlag( metaData.propFlags(), EditAnywhere )
                    && a_OverrideFlag != VisibleAnywhere )
                {
                    drawFlag = static_cast<PropertyFlags>( EditAnywhere );
                }
                else if ( HasFlag( metaData.propFlags(), VisibleAnywhere )
                    || a_OverrideFlag == VisibleAnywhere )
                {
                    drawFlag = static_cast<PropertyFlags>( VisibleAnywhere );
                }
                else
                {
                    continue;
                }

                // Get a copy of the member data from the handle
                MetaAny memberData = a_Handle.type().is_pointer_like() ? metaData.get( *a_Handle ) : metaData.get( a_Handle );

                // If the property has a draw function, call it.
                if ( auto drawFuncProp = metaData.type().prop( DrawPropFuncID ) )
                {
                    auto drawFunc = drawFuncProp.value().cast<DrawPropFunc>();
                    memberWasChanged |= drawFunc( metaData.name().c_str(), memberData, drawFlag );
                }
				// Handle the drawing of associative containers
                else if ( metaData.type().is_associative_container() )
                {
					auto associativeContainer = ( *memberData ).as_associative_container();
					if ( associativeContainer.key_only() )
					{
                        TODO( "Add property drawing for key only associative containers!" );
					}
					else
					{
						memberWasChanged |= DrawKeyToValueAssociativeContainer( associativeContainer, metaData, drawFlag );
					}
                }
				// Handle the drawing of sequence containers
                else if ( metaData.type().is_sequence_container() )
                {
                    auto sequenceContainer = ( *memberData ).as_sequence_container();
                    memberWasChanged |= DrawSequenceContainer( sequenceContainer, metaData, drawFlag );
                }
            }

            return wasChanged;
        }

	}

}

#endif // IS_EDITOR