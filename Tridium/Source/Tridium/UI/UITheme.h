#pragma once
#include <Tridium/ImGui/ImGui.h>
#include <Tridium/Graphics/Color.h>

namespace Tridium {

	//=============================================================================================
	// UI Color: A color structure specifically for UI elements, extending Color4
	//=============================================================================================
	struct UIColor : Color4
	{
		UIColor() : Color4( 0.0f, 0.0f, 0.0f, 1.0f ) {}
		UIColor( uint32_t a_Color ) { *this = ImGui::ColorConvertU32ToFloat4( a_Color ); }
		UIColor( float a_Red, float a_Green, float a_Blue, float a_Alpha = 1.0f ) : Color4( a_Red, a_Green, a_Blue, a_Alpha ) {}
		UIColor( const Color4& a_Color ) : Color4( a_Color ) {}
		UIColor( const ImVec4& a_ImVec ) : Color4( a_ImVec.x, a_ImVec.y, a_ImVec.z, a_ImVec.w ) {}
		operator ImVec4() const { return ImVec4( r, g, b, a ); }
		operator ImU32() const { return ImGui::GetColorU32( ImVec4( r, g, b, a ) ); }
	};

	#define UI_COLOR_FROM_U8( R, G, B, A ) uint32_t( ( (uint32_t)(A) << 24 ) | ( (uint32_t)(B) << 16 ) | ( (uint32_t)(G) << 8 ) | (uint32_t)(R) )

	struct UITheme
	{
		// Background
		UIColor Background{ UI_COLOR_FROM_U8( 36u, 36u, 40u, 255u ) };
		UIColor BrightBackground{ UI_COLOR_FROM_U8( 46u, 46u, 50u, 255u ) };
		UIColor SelectedBackground{ UI_COLOR_FROM_U8( 86u, 86u, 95u, 255u ) };
		UIColor DarkBackground{ UI_COLOR_FROM_U8( 26u, 26u, 30u, 255u ) };

		// Primary Colors
		UIColor Red{ UI_COLOR_FROM_U8( 255, 70, 90, 255 ) };
		UIColor Green{ UI_COLOR_FROM_U8( 170, 255, 85, 255 ) };
		UIColor Blue{ UI_COLOR_FROM_U8( 50, 150, 255, 255 ) };
		UIColor Orange{ UI_COLOR_FROM_U8( 255, 150, 50, 255 ) };

		// Text
		UIColor Text{ UI_COLOR_FROM_U8( 191, 191, 191, 255 ) };
		UIColor BrightText{ UI_COLOR_FROM_U8( 230, 230, 230, 255 ) };
		UIColor DarkText{ UI_COLOR_FROM_U8( 130, 130, 130, 255 ) };

		// Error States
		UIColor SuccessGreen{ UI_COLOR_FROM_U8( 170, 255, 85, 255 ) };
		UIColor WarningOrange{ UI_COLOR_FROM_U8( 255, 150, 50, 255 ) };
		UIColor ErrorRed{ UI_COLOR_FROM_U8( 255, 70, 90, 255 ) };
		UIColor InfoBlue{ UI_COLOR_FROM_U8( 50, 150, 255, 255 ) };
		UIColor HighlightedRed{ UI_COLOR_FROM_U8( 255, 90, 110, 255 ) };

		// Titlebar
		UIColor Titlebar{ UI_COLOR_FROM_U8( 26, 26, 30, 255 ) };
		UIColor TitlebarGradientTopLeft{ UI_COLOR_FROM_U8( 26, 26, 30, 0 ) };
		UIColor TitlebarGradientTopRight{ UI_COLOR_FROM_U8( 26, 26, 30, 0 ) };
		UIColor TitlebarGradientBottomLeft{ UI_COLOR_FROM_U8( 26, 26, 30, 0 ) };
		UIColor TitlebarGradientBottomRight{ UI_COLOR_FROM_U8( 26, 26, 30, 0 ) };

		UIColor WindowBorder{ UI_COLOR_FROM_U8( 51, 51, 51, 255 ) };

		UIColor Underline{ UI_COLOR_FROM_U8( 26, 26, 30, 255 ) };
		UIColor Accent{ UI_COLOR_FROM_U8( 255, 90, 110, 255 ) };

		// Selection
		UIColor Selected{ Blue };
		float SelectedBorderSize = 2.0f;
		float SelectedRounding = 4.0f;
	};

	namespace UI {

		const UITheme& GetTheme();
		void ApplyTheme( const UITheme& a_Theme );

	} // namespace UI

} // namespace Tridium