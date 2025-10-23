#pragma once
#include <Tridium/ImGui/ImGui.h>
#include <Tridium/UI/Config.h>
#include <Tridium/UI/UITheme.h>

#if CONFIG_ENABLE_TOOL_UI

namespace Tridium {
	
namespace UI {

	//=============================================================================================
	// Moves the drawing cursor by the specified offset.
	void ShiftCursor( float a_OffsetX, float a_OffsetY );

	//=============================================================================================
	// Generates and pushes unique ID.
	// WARNING: PushID must have a matching PopID call.
	void PushID();
	void PopID();

	//=============================================================================================
	// Draw Functions
	//=============================================================================================

	//=============================================================================================
	// Draws a horizontal line under the current cursor position.
	// If a_SpanFullWidth is true, the line will span the full available content width.
	void DrawUnderline( bool a_SpanFullWidth = false, float a_OffsetX = 0.0f, float a_OffsetY = 0.0f );

	//=============================================================================================
	// Fast draw function for a simple bezier curve between two points.
	// Use SmartBezierCollision to check for collisions with the curve.
	void DrawSmartBezier( const Vector2& a_Start, const Vector2& a_End, UIColor a_Color, float a_Thickness = 1.0f );
	bool SmartBezierCollision( const Vector2& a_Start, const Vector2& a_End, const Vector2& a_Point, float a_Radius );

	//=============================================================================================
	// Property Grid Functions
	// Any DrawProperty call between BeginPropertyGrid and EndPropertyGrid,
	// will be drawn in a property grid format.
	// a_NumColumns specifies how many columns the grid will have.
	// WARNING: BeginPropertyGrid must have a matching EndPropertyGrid call.
	void BeginPropertyGrid( uint32_t a_NumColumns = 2 );
	void EndPropertyGrid();

} // namespace UI

} // namespace Tridium:

#endif // CONFIG_ENABLE_TOOL_UI