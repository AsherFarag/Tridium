#pragma once

#include <Tridium/Core/Core.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Containers/Variant.h>
#include <Tridium/Utils/TypeTraits.h>
#include <Tridium/Input/Input.h>

namespace Tridium {

	// Events in Tridium are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then and there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.

	enum class EEventType : uint8_t
	{
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum class EEventCategory : uint8_t
	{
		None           = 0,
		Application    = 1 << 0,
		Input          = 1 << 1,
		Keyboard       = 1 << 2,
		Mouse          = 1 << 3,
		MouseButton    = 1 << 4
	};
	ENUM_ENABLE_BITMASK_OPERATORS( EEventCategory )

#define EVENT_BODY( _Name, _Type, _Category ) \
		static constexpr EEventType Type = _Type; \
		static constexpr EEventCategory Category = _Category; \
		static constexpr StringView Name = _Name; \

#pragma region Application Events

	struct WindowResizeEvent
	{
		EVENT_BODY( "WindowResize", EEventType::WindowResize, EEventCategory::Application)

		const uint32_t Width, Height;

		WindowResizeEvent( uint32_t a_Width, uint32_t a_Height )
			: Width( a_Width ), Height( a_Height ) {}

		String ToString() const
		{
			return std::format( "WindowResizeEvent: {}, {}", Width, Height );
		}
	};

	struct WindowCloseEvent
	{
		EVENT_BODY( "WindowClose", EEventType::WindowClose, EEventCategory::Application )

		String ToString() const
		{
			return "WindowClose";
		}
	};

	struct AppTickEvent
	{
		EVENT_BODY( "AppTick", EEventType::AppTick, EEventCategory::Application )
		String ToString() const
		{
			return "AppTick";
		}
	};

	struct AppUpdateEvent
	{
		EVENT_BODY( "AppUpdate", EEventType::AppUpdate, EEventCategory::Application )
		String ToString() const
		{
			return "AppUpdate";
		}
	};

	struct AppRenderEvent
	{
		EVENT_BODY( "AppRender", EEventType::AppRender, EEventCategory::Application )
		String ToString() const
		{
			return "AppRender";
		}
	};

#pragma endregion

#pragma region Keyboard Events

	struct KeyPressedEvent
	{
		EVENT_BODY( "KeyPressed", EEventType::KeyPressed, EEventCategory::Keyboard | EEventCategory::Input );

		const EInputKey KeyCode;
		const bool IsRepeat;
		KeyPressedEvent( EInputKey a_Keycode, bool a_Repeat )
			: KeyCode( a_Keycode ), IsRepeat( a_Repeat ) {}

		String ToString() const
		{
			return std::format( "Key Pressed: {}, (repeat = {})", ::Tridium::ToString( KeyCode ), IsRepeat );
		}
	};

	struct KeyReleasedEvent
	{
		EVENT_BODY( "KeyReleased", EEventType::KeyReleased, EEventCategory::Keyboard | EEventCategory::Input );

		const EInputKey KeyCode;
		KeyReleasedEvent( EInputKey a_Keycode )
			: KeyCode( a_Keycode ) {}

		String ToString() const
		{
			return std::format( "Key Released: {}", ::Tridium::ToString( KeyCode ) );
		}
	};

	struct KeyTypedEvent
	{
		EVENT_BODY( "KeyTyped", EEventType::KeyTyped, EEventCategory::Keyboard | EEventCategory::Input );

		const EInputKey KeyCode;
		KeyTypedEvent( EInputKey a_Keycode )
			: KeyCode( a_Keycode ) {}

		String ToString() const
		{
			return std::format( "Key Typed: {}", ::Tridium::ToString( KeyCode ) );
		}
	};

#pragma endregion

#pragma region Mouse Events

	struct MouseMovedEvent
	{
		EVENT_BODY( "MouseMoved", EEventType::MouseMoved, EEventCategory::Mouse | EEventCategory::Input );

		const float X, Y;
		MouseMovedEvent( float a_X, float a_Y )
			: X( a_X ), Y( a_Y ) {}

		String ToString() const
		{
			return std::format( "Mouse Moved to: x {}, y {}", X, Y );
		}
	};

	struct MouseScrolledEvent
	{
		EVENT_BODY( "MouseScrolled", EEventType::MouseScrolled, EEventCategory::Mouse | EEventCategory::Input );

		const float XOffset, YOffset;
		MouseScrolledEvent( float a_XOffset, float a_YOffset )
			: XOffset( a_XOffset ), YOffset( a_YOffset ) {}

		String ToString() const
		{
			return std::format( "Mouse Scrolled: {}, {}", XOffset, YOffset );
		}
	};

	struct MouseButtonPressedEvent
	{
		EVENT_BODY( "MouseButton", EEventType::MouseButtonPressed, EEventCategory::Mouse | EEventCategory::Input | EEventCategory::MouseButton );

		const EInputMouseButton Button;
		MouseButtonPressedEvent( EInputMouseButton a_Button )
			: Button( a_Button ) {}

		String ToString() const
		{
			return std::format( "Mouse Button Pressed: {}", ::Tridium::ToString( Button ) );
		}
	};

	struct MouseButtonReleasedEvent
	{
		EVENT_BODY( "MouseButton", EEventType::MouseButtonReleased, EEventCategory::Mouse | EEventCategory::Input | EEventCategory::MouseButton );

		const EInputMouseButton Button;
		MouseButtonReleasedEvent( EInputMouseButton a_Button )
			: Button( a_Button ) {}

		String ToString() const
		{
			return std::format( "Mouse Button Released: {}", ::Tridium::ToString( Button ) );
		}
	};

#pragma endregion

	using EventVariant = Variant<
		WindowCloseEvent,
		WindowResizeEvent,
		AppTickEvent,
		AppUpdateEvent,
		AppRenderEvent,
		KeyPressedEvent,
		KeyReleasedEvent,
		KeyTypedEvent,
		MouseMovedEvent,
		MouseScrolledEvent,
		MouseButtonPressedEvent,
		MouseButtonReleasedEvent
	>;


	template < typename T, typename _Var >
	struct DoesVariantContain : std::false_type {};

	template < typename T, typename... _Args >
	struct DoesVariantContain< T, Variant< _Args... > >
	{
		static constexpr bool value = (std::is_same_v<T, _Args> || ...);
	};

	template<typename T>
	concept IsEventType = DoesVariantContain<T, EventVariant>::value;

	struct Event
	{
		EventVariant Data{};
		bool Handled = false;

		EEventType Type() const
		{
			return std::visit( []( const auto& a_Event ) { return a_Event.Type; }, Data );
		}

		StringView Name() const
		{
			return std::visit( []( const auto& a_Event ) { return a_Event.Name; }, Data );
		}

		EEventCategory Category() const
		{
			return std::visit( []( const auto& a_Event ) { return a_Event.Category; }, Data );
		}

		String ToString() const
		{
			return std::visit( []( const auto& a_Event ) { return a_Event.ToString(); }, Data );
		}

		bool IsInCategory( EEventCategory a_Category ) const
		{
			return EnumFlags( Category() ).HasFlag( a_Category );
		}

		template<IsEventType T>
		bool Is() const
		{
			return std::holds_alternative<T>( Data );
		}

		template<EEventType _Type>
		bool Is() const
		{
			return std::holds_alternative<Cast<size_t>( _Type )>( Data );
		}

		template<IsEventType T>
		T& Get()
		{
			return std::get<T>( Data );
		}

		template<IsEventType T>
		const T& Get() const
		{
			return std::get<T>( Data );
		}

		template<EEventType _Type>
		const auto& Get() const
		{
			return std::get<Cast<size_t>( _Type )>( Data );
		}

		Event() = default;

		template<IsEventType T>
		Event( const T& a_Event ) 
			: Data( a_Event ) {}
	};

	class EventDispatcher
	{
	public:
		EventDispatcher( Event& a_Event ) : m_Event( a_Event ) {}
		
		template<IsEventType _EventType, typename _Func>
		bool Dispatch( const _Func& a_Func )
		{
			if ( m_Event.Is<_EventType>() )
			{
				m_Event.Handled |= a_Func( m_Event.Get<_EventType>() );
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};
}
