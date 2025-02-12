#pragma once
#include <any>
#include <typeinfo>

namespace Tridium {

    class Any
    {
    public:
        Any() = default;

        template<typename T>
        Any( const T& a_Value )
        {
            m_Any = a_Value;
        }

        Any( Any&& a_Other ) noexcept
        {
            m_Any = std::move( a_Other.m_Any );
        }

        Any( const Any& a_Other )
        {
            m_Any = a_Other.m_Any;
        }

        Any& operator=( Any&& a_Other ) noexcept
        {
            if ( this != &a_Other )
            {
                m_Any = std::move( a_Other.m_Any );
            }
            return *this;
        }

        Any& operator=( const Any& a_Other )
        {
            if ( this != &a_Other )
            {
                m_Any = a_Other.m_Any;
            }
            return *this;
        }

		// Get the stored value as type T
        template<typename T>
        T As() const
        {
            return std::any_cast<T>( m_Any );
        }

		// Get the stored value as a reference to type T
        template<typename T>
        T& AsRef()
        {
            return std::any_cast<T&>( m_Any );
        }

        // Check if the Any object has a stored value
        bool HasValue() const
        {
            return m_Any.has_value();
        }

        // Get the type info of the stored value
        const std::type_info& Type() const
        {
            return m_Any.type();
        }

		// Reset the stored value to an empty state
        void Reset()
        {
            m_Any.reset();
        }

        // Swap contents with another Any object
        void Swap( Any& a_Other )
        {
            m_Any.swap( a_Other.m_Any );
        }

        // Set the stored value to a new value of type T
        template<typename T>
        void Set( const T& a_Value )
        {
            m_Any = a_Value;
        }

    private:
        std::any m_Any;
    };


} // namespace Tridium