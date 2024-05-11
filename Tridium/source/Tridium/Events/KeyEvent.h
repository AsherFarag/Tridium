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
		KeyPressedEvent( Input::KeyCode a_Keycode, int a_RepeatCount )
			: KeyEvent( a_Keycode ), m_RepeatCount( a_RepeatCount ) {}

		inline int GetRepeatCount() const { return m_KeyCode; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_RepeatCount << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
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
