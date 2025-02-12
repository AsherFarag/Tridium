#include "tripch.h"
#if IS_EDITOR

#include "EditorReflection.h"
#include "Reflection.h"
#include <Tridium/ImGui/IconsFontAwesome6.h>

namespace Tridium {
    // Removes m_ and adds spaces in between words in a Pascal Case string
	// Example: m_MyVariableName -> My Variable Name
    static std::string ScrubPropertyName( const std::string& name )
    {
		std::string scrubbedName = name;
		// Remove m_ from the start of the string
		if ( scrubbedName.size() > 2 && scrubbedName[0] == 'm' && scrubbedName[1] == '_' )
		{
			scrubbedName.erase( 0, 2 );
		}

		for ( size_t i = 1; i < scrubbedName.size(); ++i )
		{
			if ( std::isupper( scrubbedName[i] ) )
			{
				scrubbedName.insert( i, " " );
				++i;
			}
		}

		return scrubbedName;
    }

	namespace Refl::Internal {
        using enum ::Tridium::Refl::EPropertyFlags;

#pragma region Helpers

        struct ElementReturn
        {
			uint8_t IsOpen : 1;
			uint8_t WasRemoved : 1;
        };

        bool DrawRemoveElementButton( const char* a_Name )
        {
            ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ( ImGui::CalcTextSize( a_Name ).x + 10 ) );
			ImGui::ScopedStyleCol col( ImGuiCol_Button, Style::Colors::Red.Value );
            return ImGui::SmallButton( a_Name );
        }

        ElementReturn BeginElementTreeNode( const char* a_Name, EPropertyFlags a_Flags )
        {
            ElementReturn ret;

            ret.IsOpen = ImGui::TreeNodeEx( a_Name, ImGuiTreeNodeFlags_DefaultOpen );

            if ( HasFlag( a_Flags, EPropertyFlags::EditAnywhere ) )
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

        bool DrawKeyToValueAssociativeContainer( entt::meta_associative_container& a_AssociativeContainer, const MetaProp& a_MetaData, EPropertyFlags a_DrawFlag )
        {
			MetaType keyType = a_AssociativeContainer.key_type();
			MetaType valueType = a_AssociativeContainer.mapped_type();

            const bool isOpen = ImGui::TreeNodeEx( ScrubPropertyName( a_MetaData.name() ).c_str() );

            if ( ImGui::BeginItemTooltip() )
            {
                ImGui::Text( "Class: Map Container of '%s' to '%s'",
					keyType.GetCleanTypeName(),
					valueType.GetCleanTypeName() );
                ImGui::EndTooltip();
            }

			if ( !isOpen )
				return false;

            ImGui::FunctionScope treePop( +[]() { ImGui::TreePop(); } );

            bool wasChanged = false;

			// Get the draw functions for the key and value
            Props::DrawPropertyProp::Type keyDrawFunc = nullptr;
			Props::DrawPropertyProp::Type valueDrawFunc = nullptr;

            if ( auto meta = keyType.GetMetaAttribute( Props::DrawPropertyProp::ID ) )
                keyDrawFunc = meta.value().cast<Props::DrawPropertyProp::Type>();
            else
                return false;

			if ( auto meta = valueType.GetMetaAttribute( Props::DrawPropertyProp::ID ) )
                valueDrawFunc = meta.value().cast<Props::DrawPropertyProp::Type>();
			else
				return false;


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

            if ( !HasFlag(a_DrawFlag, EPropertyFlags::EditAnywhere) 
                && a_AssociativeContainer.size() == 0 )
            {
                ImGui::Text( "Empty" );
            }

            if ( HasFlag( a_DrawFlag, EPropertyFlags::EditAnywhere )
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

        bool DrawSequenceContainer( entt::meta_sequence_container& a_SequenceContainer, const MetaProp& a_MetaData, EPropertyFlags a_DrawFlag )
        {
			MetaType elemType = a_SequenceContainer.value_type();

            const bool isOpen = ImGui::TreeNodeEx( ScrubPropertyName( a_MetaData.name() ).c_str() );

            if ( ImGui::BeginItemTooltip() )
            {
                ImGui::Text( "Class: Sequence Container of '%s'", elemType.GetCleanTypeName() );
                ImGui::EndTooltip();
            }

			if ( !isOpen )
				return false;

            bool wasChanged = false;

			Props::DrawPropertyProp::Type drawFunc = nullptr;

			if ( auto meta = elemType.GetMetaAttribute( Props::DrawPropertyProp::ID ) )
				drawFunc = meta.value().cast<Props::DrawPropertyProp::Type>();

            if ( drawFunc )
            {
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

					if ( HasFlag( a_DrawFlag, EPropertyFlags::EditAnywhere ) )
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

			if ( HasFlag( a_DrawFlag, EPropertyFlags::EditAnywhere ) )
            {
                if ( DrawAddElementButton( "Add Element" ) )
                {
					auto newElement = elemType.Construct();
                    a_SequenceContainer.insert( a_SequenceContainer.end(), std::move(newElement) );
                    wasChanged = true;
                }
            }

			if ( !HasFlag( a_DrawFlag, EPropertyFlags::EditAnywhere ) 
                && a_SequenceContainer.size() == 0 )
			{
				ImGui::Text( "Empty" );
			}

            ImGui::TreePop();

            return wasChanged;
        }

		bool DrawAllMembersOfMetaClass( const MetaType& a_MetaType, MetaAny& a_Handle, EPropertyFlags a_OverrideFlag )
        {
            bool wasChanged = false;

			// Make the second column consume the remaining space
			if ( !ImGui::BeginTable( "##Table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable ) )
				return false;

            for ( auto&& [id, metaData] : a_MetaType.Properties() )
            {
                bool memberWasChanged = false;
                EPropertyFlags drawFlag = EPropertyFlags::EPF_None;

                // Get the appropiate draw flag
                // If the draw flag is none, skip the property
                if ( HasFlag( metaData.propFlags(), EditAnywhere )
                    && a_OverrideFlag != VisibleAnywhere )
                {
                    drawFlag = EditAnywhere;
                }
                else if ( HasFlag( metaData.propFlags(), VisibleAnywhere )
                    || a_OverrideFlag == VisibleAnywhere )
                {
                    drawFlag = VisibleAnywhere;
                }
                else
                {
                    continue;
                }

                // Get a copy of the member data from the handle
                MetaAny memberData = a_Handle.type().is_pointer_like() ? metaData.get( *a_Handle ) : metaData.get( a_Handle );
				MetaType memberType = memberData.type().is_pointer_like() ? memberData.type().remove_pointer() : memberData.type();

				// Draw the property name on the left side of the table
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextUnformatted( ScrubPropertyName( metaData.name() ).c_str() );

				// Draw the property on the right side of the table
				ImGui::TableNextColumn();
				ImGui::PushItemWidth( -1 );
                {
					ImGui::PushID( metaData.name().c_str() );
                    // If the property has a draw function, call it.
                    if ( auto drawFuncProp = memberType.GetMetaAttribute( Props::DrawPropertyProp::ID ) )
                    {
                        auto drawFunc = drawFuncProp.value().cast<Props::DrawPropertyProp::Type>();
                        memberWasChanged |= drawFunc( "", memberData, drawFlag );
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
					ImGui::PopID();
                }

				ImGui::PopItemWidth();
            }

			ImGui::EndTable();

            return wasChanged;
        }

	}

}

#endif // IS_EDITOR