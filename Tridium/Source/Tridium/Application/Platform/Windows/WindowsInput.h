#pragma once
#include <Tridium/Input/Input.h>

namespace Tridium {

	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl( int keycode ) override;

		virtual bool IsMouseButtonPressedImpl( int button ) override;
		virtual Vector2 GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual void SetInputModeImpl( EInputMode mode, EInputModeValue value ) override;
	};

}