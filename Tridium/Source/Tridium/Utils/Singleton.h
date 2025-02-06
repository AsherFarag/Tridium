#pragma once
#include <type_traits>
#include <Tridium/Core/Assert.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

    //==========================================================================
	// ISingleton
    //  Singleton class for single-instance objects. If ExplicitSetup is
    //  disabled, T must be default constructible, otherwise it will need to be
    //  constructed beforehand.
    //  
	//  _ExplicitSetup:
    //      If true, the singleton must be explicitly constructed.
	//      If false, the singleton will be constructed on first access. This requires T to be default constructable.
    //==========================================================================
    template < typename T, bool _ExplicitSetup = true >
	class ISingleton
    {
    private:
        struct Deleter
        {
            void operator()( T* a_Ptr ) const;
        };

    public:
		// Nested Singleton class for namespace encapsulation.
		// This class is used to manage the global instance of the singleton.
        class Singleton 
        {
		public:
            static constexpr bool ExplicitSetup = _ExplicitSetup;

            // Get a reference to the global instance for this singleton.
            // If not explicitly initialisable, then an instance will be created via default-construction.
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

            // Creates the singleton's global instance, taking in constructor args.
            template< typename... _Args >
            static bool Construct( _Args&&... a_Args )
            {
                if ( s_Instance )
                {
                    return false;
                }

				Destroy();
				s_Instance.reset( new T( std::forward<_Args>( a_Args )... ) );
                s_Instance.get_deleter() = Deleter{};
                s_Instance->OnSingletonConstructed();

                return true;
            }

			// Bind an existing instance to the singleton.
            static void BindExisting( UniquePtr<T> a_Instance )
            {
				if ( !ASSERT( a_Instance ) )
				{
					return;
				}

				Destroy();
				s_Instance = std::move( a_Instance );
                s_Instance.get_deleter() = Deleter{};
				s_Instance->OnSingletonConstructed();
				return;
            }

            // Will remove ownership to existing singleton object without deleting.
			// WARNING: Callers must ensure the object is deleted elsewhere.
            static T* Release()
            {
                return s_Instance.release();
            }

            // Destroy the global instance and reset to null. Construct/BindExisting must be called again if explicitly initialisable.
            static void Destroy()
            {
				if ( !s_Instance )
				{
					return;
				}

                s_Instance->OnSingletonDestroyed();
                s_Instance.reset();
            }

			// Does the global instance exist?
			static bool Exists() { return s_Instance != nullptr; }

            // Does the global instance exist and is it valid?
            static bool IsValid() { return Exists() && s_Instance->IsValidSingleton(); }
            
		private:
            static UniquePtr<T, Deleter> s_Instance;
        };

        // Get a reference to the global instance for this singleton.
        // If not explicitly initialisable, then an instance will be created via default-construction.
        static T* Get()
        {
			return Singleton::Get();
        }

    protected:
        virtual ~ISingleton() = default;

        // Fired when an existing or new singleton is bound or constructed.
        virtual void OnSingletonConstructed() {}

		// Fired when the singleton is destroyed.
		virtual void OnSingletonDestroyed() {}

		// Is the singleton instance valid?
		virtual bool IsValidSingleton() const { return true; }

		//======================================================================

        friend class Singleton;
		friend struct Deleter;
    };

    //==========================================================================
	// Define the static instance of the singleton.
    template<typename T, bool _ExplicitSetup>
	UniquePtr<T, typename ISingleton<T, _ExplicitSetup>::Deleter> ISingleton<T, _ExplicitSetup>::Singleton::s_Instance = nullptr;

    template<typename T, bool _ExplicitSetup>
    inline void ISingleton<T, _ExplicitSetup>::Deleter::operator()( T* a_Ptr ) const
    {
		a_Ptr->OnSingletonDestroyed();
		delete a_Ptr;
    }

} // namespace Tridium