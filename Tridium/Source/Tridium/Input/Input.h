#pragma once

#include <Tridium/Core/Core.h>

namespace Tridium {

	enum class EInputMode
	{
		None = 0,
		Cursor,
		Sticky_Keys,
		Sticky_Mouse_Buttons,
		Lock_Key_Mods,
		Raw_Mouse_Motion,
		Unlimited_Mouse_Buttons
	};

	TODO( "Finish the rest of these!" );
	enum class EInputModeValue
	{
		False = 0,
		True = 1,
		Cursor_Normal,
		Cursor_Hidden,
		Cursor_Disabled,
		Cursor_Captured,
	};

	enum class EInputKey;
	enum class EInputMouseButton;

	class Input
	{
		friend class Application;
	public:
		static Input* Get() { return s_Instance.get(); }

		// Returns true if this key is down this frame
		inline static bool IsKeyPressed( EInputKey a_Keycode ) { return Get()->IsKeyPressedImpl( static_cast<int>( a_Keycode ) ); }

        inline static bool IsMouseButtonPressed( EInputMouseButton a_Button ) { return Get()->IsMouseButtonPressedImpl( static_cast<int>( a_Button ) ); }
		inline static Vector2 GetMousePosition() { return Get()->GetMousePositionImpl(); }
		inline static float GetMouseX() { return Get()->GetMouseXImpl(); }
		inline static float GetMouseY() { return Get()->GetMouseYImpl(); }
		inline static void SetInputMode( EInputMode a_Mode, EInputModeValue a_Value ) { Get()->SetInputModeImpl( a_Mode, a_Value ); }
		inline static int GetMouseScrollXOffset() { return Get()->m_MouseScrollXOffset; }
		inline static int GetMouseScrollYOffset() { return Get()->m_MouseScrollYOffset; }

		// DO NOT USE!
		// For internal use only.
		inline static void _SetMouseScrollOffset( int x, int y ) { Get()->m_MouseScrollXOffset += x; Get()->m_MouseScrollYOffset += y; }

	protected:
		virtual bool IsKeyPressedImpl( int keycode ) = 0;

		virtual bool IsMouseButtonPressedImpl( int button ) = 0;
		virtual Vector2 GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual void SetInputModeImpl( EInputMode mode, EInputModeValue value ) = 0;

	private:
		int m_MouseScrollXOffset = 0;
		int m_MouseScrollYOffset = 0;

		static UniquePtr<Input> s_Instance;
	};

	enum class EInputMouseButton : int32_t
	{
		Button1 = 0,
		Button2 = 1,
		Button3 = 2,
		Button4 = 3,
		Button5 = 4,
		Button6 = 5,
		Button7 = 6,
		Button8 = 7,
        None,
		Left = Button1,
		Right = Button2,
		Middle = Button3
	};

    enum class EInputKey : int32_t
    {
        /* Printable keys */
        InvalidKey = -1,
        Space = 32,
        Apostrophe = 39,  /* ' */
        Comma = 44,  /* , */
        Minus = 45,  /* - */
        Period = 46,  /* . */
        Slash = 47,  /* / */
        Zero = 48,
        One = 49,
        Two = 50,
        Three = 51,
        Four = 52,
        Five = 53,
        Six = 54,
        Seven = 55,
        Eight = 56,
        Nine = 57,
        Semicolon = 59,  /* ; */
        Equal = 61,  /* = */
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,  /* [ */
        Backslash = 92,  /* \ */
        RightBracket = 93,  /* ] */
        GraveAccent = 96,  /* ` */
        World1 = 161, /* non-US #1 */
        World2 = 162, /* non-US #2 */

        /* Function keys */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        KeyPad0 = 320,
        KeyPad1 = 321,
        KeyPad2 = 322,
        KeyPad3 = 323,
        KeyPad4 = 324,
        KeyPad5 = 325,
        KeyPad6 = 326,
        KeyPad7 = 327,
        KeyPad8 = 328,
        KeyPad9 = 329,
        KeyPadDecimal = 330,
        KeyPadDivide = 331,
        KeyPadMultiply = 332,
        KeyPadSubtract = 333,
        KeyPadAdd = 334,
        KeyPadEnter = 335,
        KeyPadEqual = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343, /* E.g. Windows Key */
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
    };

	// Converts an EInputMouseButton to a string
	constexpr StringView ToString( EInputMouseButton a_Button )
	{
		switch ( a_Button )
		{
		case EInputMouseButton::Left:    return "Left";
		case EInputMouseButton::Right:   return "Right";
		case EInputMouseButton::Middle:  return "Middle";
		case EInputMouseButton::Button4: return "Button4";
		case EInputMouseButton::Button5: return "Button5";
		case EInputMouseButton::Button6: return "Button6";
		case EInputMouseButton::Button7: return "Button7";
		case EInputMouseButton::Button8: return "Button8";
		default:                         return "Unknown";
		}
	}

	// Converts an EInputKey to a string
	constexpr StringView ToString( EInputKey a_Key )
	{
		switch ( a_Key )
		{
		case EInputKey::Space: return "Space";
		case EInputKey::Apostrophe: return "Apostrophe";
		case EInputKey::Comma: return "Comma";
		case EInputKey::Minus: return "Minus";
		case EInputKey::Period: return "Period";
		case EInputKey::Slash: return "Slash";
		case EInputKey::Zero: return "0";
		case EInputKey::One: return "1";
		case EInputKey::Two: return "2";
		case EInputKey::Three: return "3";
		case EInputKey::Four: return "4";
		case EInputKey::Five: return "5";
		case EInputKey::Six: return "6";
		case EInputKey::Seven: return "7";
		case EInputKey::Eight: return "8";
		case EInputKey::Nine: return "9";
		case EInputKey::Semicolon: return "Semicolon";
		case EInputKey::Equal: return "Equal";
		case EInputKey::A: return "A";
		case EInputKey::B: return "B";
		case EInputKey::C: return "C";
		case EInputKey::D: return "D";
		case EInputKey::E: return "E";
		case EInputKey::F: return "F";
		case EInputKey::G: return "G";
		case EInputKey::H: return "H";
		case EInputKey::I: return "I";
		case EInputKey::J: return "J";
		case EInputKey::K: return "K";
		case EInputKey::L: return "L";
		case EInputKey::M: return "M";
		case EInputKey::N: return "N";
		case EInputKey::O: return "O";
		case EInputKey::P: return "P";
		case EInputKey::Q: return "Q";
		case EInputKey::R: return "R";
		case EInputKey::S: return "S";
		case EInputKey::T: return "T";
		case EInputKey::U: return "U";
		case EInputKey::V: return "V";
		case EInputKey::W: return "W";
		case EInputKey::X: return "X";
		case EInputKey::Y: return "Y";
		case EInputKey::Z: return "Z";
		case EInputKey::LeftBracket:      return "LeftBracket";
		case EInputKey::Backslash:        return "Backslash";
		case EInputKey::RightBracket:     return "RightBracket";
		case EInputKey::GraveAccent:      return "GraveAccent";
		case EInputKey::World1:           return "World1";
		case EInputKey::World2:           return "World2";
		case EInputKey::Escape:           return "Escape";
		case EInputKey::Enter:            return "Enter";
		case EInputKey::Tab:              return "Tab";
		case EInputKey::Backspace:        return "Backspace";
		case EInputKey::Insert:           return "Insert";
		case EInputKey::Delete:           return "Delete";
		case EInputKey::Right:            return "Right";
		case EInputKey::Left:             return "Left";
		case EInputKey::Down:             return "Down";
		case EInputKey::Up:               return "Up";
		case EInputKey::PageUp:           return "PageUp";
		case EInputKey::PageDown:         return "PageDown";
		case EInputKey::Home:             return "Home";
		case EInputKey::End:              return "End";
		case EInputKey::CapsLock:         return "CapsLock";
		case EInputKey::ScrollLock:       return "ScrollLock";
		case EInputKey::NumLock:          return "NumLock";
		case EInputKey::PrintScreen:      return "PrintScreen";
		case EInputKey::Pause:            return "Pause";
		case EInputKey::F1:               return "F1";
		case EInputKey::F2:               return "F2";
		case EInputKey::F3:               return "F3";
		case EInputKey::F4:               return "F4";
		case EInputKey::F5:               return "F5";
		case EInputKey::F6:               return "F6";
		case EInputKey::F7:               return "F7";
		case EInputKey::F8:               return "F8";
		case EInputKey::F9:               return "F9";
		case EInputKey::F10:              return "F10";
		case EInputKey::F11:              return "F11";
		case EInputKey::F12:              return "F12";
		case EInputKey::F13:              return "F13";
		case EInputKey::F14:              return "F14";
		case EInputKey::F15:              return "F15";
		case EInputKey::F16:              return "F16";
		case EInputKey::F17:              return "F17";
		case EInputKey::F18:              return "F18";
		case EInputKey::F19:              return "F19";
		case EInputKey::F20:              return "F20";
		case EInputKey::F21:              return "F21";
		case EInputKey::F22:              return "F22";
		case EInputKey::F23:              return "F23";
		case EInputKey::F24:              return "F24";
		case EInputKey::F25:              return "F25";
		case EInputKey::KeyPad0:          return "KeyPad0";
		case EInputKey::KeyPad1:          return "KeyPad1";
		case EInputKey::KeyPad2:          return "KeyPad2";
		case EInputKey::KeyPad3:          return "KeyPad3";
		case EInputKey::KeyPad4:          return "KeyPad4";
		case EInputKey::KeyPad5:          return "KeyPad5";
		case EInputKey::KeyPad6:          return "KeyPad6";
		case EInputKey::KeyPad7:          return "KeyPad7";
		case EInputKey::KeyPad8:          return "KeyPad8";
		case EInputKey::KeyPad9:          return "KeyPad9";
		case EInputKey::KeyPadDecimal:    return "KeyPadDecimal";
		case EInputKey::KeyPadDivide:     return "KeyPadDivide";
		case EInputKey::KeyPadMultiply:   return "KeyPadMultiply";
		case EInputKey::KeyPadSubtract:   return "KeyPadSubtract";
		case EInputKey::KeyPadAdd:        return "KeyPadAdd";
		case EInputKey::KeyPadEnter:      return "KeyPadEnter";
		case EInputKey::KeyPadEqual:      return "KeyPadEqual";
		case EInputKey::LeftShift:        return "LeftShift";
		case EInputKey::LeftControl:      return "LeftControl";
		case EInputKey::LeftAlt:          return "LeftAlt";
		case EInputKey::LeftSuper:        return "LeftSuper";
		case EInputKey::RightShift:       return "RightShift";
		case EInputKey::RightControl:     return "RightControl";
		case EInputKey::RightAlt:         return "RightAlt";
		case EInputKey::RightSuper:       return "RightSuper";
		case EInputKey::Menu:             return "Menu";
		default: return "Unknown";
		}
	}
}