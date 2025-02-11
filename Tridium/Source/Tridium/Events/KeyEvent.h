#pragma once
#include "Tridium/Events/Event.h"
#include "Tridium/Input/Input.h"

namespace Tridium {

	class KeyEvent : public Event
	{
	public:
		inline Input::KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent( Input::KeyCode a_Keycode) : m_KeyCode(a_Keycode) {}

		Input::KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent( Input::KeyCode a_Keycode, bool a_Repeat )
			: KeyEvent( a_Keycode ), m_IsRepeat( a_Repeat ) {}

		inline bool IsRepeat() const { return m_IsRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent( Input::KeyCode a_Keycode )
			: KeyEvent( a_Keycode ) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent( Input::KeyCode a_Keycode )
			: KeyEvent( a_Keycode ) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}