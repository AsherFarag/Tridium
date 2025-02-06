#pragma once
#include <type_traits>
#include <Tridium/Core/Assert.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Utils/Concepts.h>

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
		//static_assert( ExplicitSetup || Concepts::IsDefaultConstructable<T>, "Singleton must be default constructable if ExplicitSetup is disabled." );

        // Is this singleton valid, that is, is it enabled.
        virtual bool IsValidSingleton() const { return true; }

        // This function is fired when an existing or new singleton is bound or constructed.
        virtual void OnPostSingletonConstructed() {}

        // Creates the singleton's global instance, taking in constructor args.
        template< typename... _Args >
        static bool Construct( _Args&&... a_Args )
        {
            if ( s_Instance )
            {
                return false;
            }

            s_Instance = MakeUnique<T>( std::forward< _Args >( a_Args )... );
            s_Instance->OnPostSingletonConstructed();
            return true;
        }

        // Will remove the reference to existing singleton object without deleting.
        static T* Release()
        {
			return s_Instance.release();
        }

        // Destroy the global instance and reset to null. Construct/BindExisting must be called again if explicitly initialisable.
        static void Destroy()
        {
			s_Instance.reset();
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

            CHECK( IsValid() );
            return s_Instance.get();
        }

    private:
        static UniquePtr<T> s_Instance;
    };

    //==========================================================================
    template< typename T, bool _ExplicitSetup >
    UniquePtr<T> ISingleton< T, _ExplicitSetup >::s_Instance = nullptr;

} // namespace Tridium