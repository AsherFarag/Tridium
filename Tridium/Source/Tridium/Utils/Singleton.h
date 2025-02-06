#pragma once
#include <type_traits>
#include <Tridium/Core/Assert.h>

namespace Tridium {
    //==========================================================================
// Singleton class for single-instance objects. If ExplicitSetup is
// disabled, T must be default constructible, otherwise it will need to be
// constructed beforehand.
//==========================================================================
    template < typename T, bool _ExplicitSetup = true >
    class ISingleton
    {
    protected:

        virtual ~ISingleton() = default;

    public:
        static constexpr bool ExplicitSetup = _ExplicitSetup;

        // Is this singleton valid, that is, is it enabled.
        virtual bool IsValidSingleton() const { return true; }

        // This function is fired when an existing or new singleton is bound or constructed.
        virtual void OnPostSingletonConstructed() {}

        // Creates the singleton's global instance, taking in constructor args.
        template< typename ...Args >
        static bool Construct( Args&&... a_Args )
        {
            if ( !CORE_ASSERT( !s_Instance ) )
            {
                return false;
            }

            s_Instance = new T( std::forward< Args >( a_Args )... );
            s_Instance->OnPostSingletonConstructed();
            return true;
        }

        // Cause this singleton interface to reference an external instance constructed via other means.
        // Useful for objects created by a global database, where only 1 instance of T is guaranteed.
        static bool BindExisting( T* a_Instance )
        {
            if ( !CORE_ASSERT( !s_Instance ) )
            {
                return false;
            }

            s_Instance = a_Instance;
            s_Instance->OnPostSingletonConstructed();
            return true;
        }

        // Will remove the reference to existing singleton object without deleting it first.
        static bool UnbindExisting()
        {
            if ( !CORE_ASSERT( s_Instance ) )
            {
                return false;
            }

            s_Instance = nullptr;
            return true;
        }

        // Destroy the global instance and reset to null. Construct/BindExisting must be called again if explicitly initialisable.
        static void Destroy()
        {
            if ( !s_Instance )
            {
                return;
            }

            delete s_Instance;
            s_Instance = nullptr;
        }

        // Does the global instance exist and is it valid?
        static bool IsValid() { return s_Instance && s_Instance->IsValidSingleton(); }

        // Get a reference to the global instance for this singleton. If not explicitly initialisable, then an instance will be created
        // via default-construction.
        static T* Get()
        {
            if constexpr ( !ExplicitSetup )
            {
                if ( !s_Instance )
                {
                    Construct();
                }
            }

            CORE_CHECK( IsValid() );
            return s_Instance;
        }

    private:
        static T* s_Instance;
    };

    //==========================================================================
    template< typename T, bool _ExplicitSetup >
    T* ISingleton< T, _ExplicitSetup >::s_Instance = nullptr;

} // namespace Tridium