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
    // 
	//  _IsOwning:
	//      If true, the singleton will be a UniquePtr.
	//      If false, the singleton will be a raw pointer.
    // 
	//  _IsThreadSafe:
	//      If true, the singleton will be thread-safe accessible via the Get() method.
    //==========================================================================
	template < typename T, bool _ExplicitSetup = true, bool _IsOwning = true, bool _IsThreadSafe = true >
	class ISingleton
    {
    private:
        struct Deleter
        {
            void operator()( T* a_Ptr ) const;
        };

		// If _IsOwning is true, then the pointer type will be a unique_ptr.
		// Otherwise, it will be a raw pointer.
        using PointerType = std::conditional_t<_IsOwning, UniquePtr<T, Deleter>, T*>;

		// Workaround for friend access to private members.
		using ThisType = ISingleton<T, _ExplicitSetup, _IsOwning, _IsThreadSafe>;

    public:
		// Nested Singleton class for namespace encapsulation.
		// This class is used to manage the global instance of the singleton.
        class Singleton 
        {
		public:
            static constexpr bool ExplicitSetup = _ExplicitSetup;
			static constexpr bool IsOwning = _IsOwning;
			static constexpr bool IsThreadSafe = _IsThreadSafe;

            // Creates the singleton's global instance, taking in constructor args.
            template< typename... _Args >
            static bool Construct( _Args&&... a_Args )
            {
                if ( s_Instance )
                {
                    return false;
                }

				Destroy();
				if constexpr ( IsOwning )
				{
					s_Instance.reset( new T( std::forward<_Args>( a_Args )... ) );
                    s_Instance.get_deleter() = Deleter{};
					static_cast<ThisType*>( s_Instance.get() )->OnSingletonConstructed();
				}
				else
				{
					s_Instance = new T( std::forward<_Args>( a_Args )... );
                    static_cast<ThisType*>(s_Instance)->OnSingletonConstructed();
				}

                return true;
            }

			// Bind an existing instance to the singleton.
            static void BindExisting( T* a_Instance )
            {
				if ( !ASSERT( a_Instance ) )
				{
					return;
				}

				Destroy();
				if constexpr ( IsOwning )
				{
					s_Instance.reset( a_Instance );
					s_Instance.get_deleter() = Deleter{};
					static_cast<ThisType*>( s_Instance.get() )->OnSingletonConstructed();
				}
				else
				{
					s_Instance = a_Instance;
					static_cast<ThisType*>( s_Instance )->OnSingletonConstructed();
				}
            }

            // Will remove ownership to existing singleton object without deleting.
			// WARNING: Callers must ensure the object is deleted elsewhere.
            static T* Release()
            {
				if constexpr ( IsOwning )
				{
					return s_Instance.release();
				}
				else
                {
					return std::exchange( s_Instance, nullptr );
				}
            }

            // Destroy the global instance and reset to null. Construct/BindExisting must be called again if explicitly initialisable.
            static void Destroy()
            {
				if ( !s_Instance )
				{
					return;
				}

				static_cast<ThisType&>( *s_Instance ).OnSingletonDestroyed();

				if constexpr ( IsOwning )
				{
					s_Instance.reset();
				}
				else
				{
					delete s_Instance;
					s_Instance = nullptr;
				}
            }

			// Does the global instance exist?
			static bool Exists() { return s_Instance != nullptr; }

            // Does the global instance exist and is it valid?
            static bool IsValid() 
            {
                return Exists() 
                    && static_cast<ThisType&>(*s_Instance).IsValidSingleton();
            }

		private:
            // Get a reference to the global instance for this singleton.
            // If not explicitly initialisable, then an instance will be created via default-construction.
            static T* InternalGet()
            {
                if constexpr ( !ExplicitSetup )
                {
                    if ( !s_Instance )
                    {
                        Construct();
                    }
                }

                CHECK( IsValid() );

                if constexpr ( IsOwning )
                {
                    return s_Instance.get();
                }
                else
                {
					return s_Instance;
                }
            }

            friend ThisType;
            
		private:
            static PointerType s_Instance;

        };

        // Get a reference to the global instance for this singleton.
        // If not explicitly initialisable, then an instance will be created via default-construction.
        static T* Get()
        {
			if constexpr ( Singleton::IsThreadSafe )
			{
				static std::mutex s_Mutex;
				std::lock_guard<std::mutex> lock( s_Mutex );
				return Singleton::InternalGet();
			}
			else
			{
				return Singleton::InternalGet();
			}
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

        friend Singleton;
		friend Deleter;
    };

    //==========================================================================
	// Define the static instance of the singleton.
    template<typename T, bool _ExplicitSetup, bool _IsOwning, bool _IsThreadSafe>
    ISingleton<T, _ExplicitSetup, _IsOwning>::PointerType ISingleton<T, _ExplicitSetup, _IsOwning, _IsThreadSafe>::Singleton::s_Instance = nullptr;

	template<typename T, bool _ExplicitSetup, bool _IsOwning, bool _IsThreadSafe>
    inline void ISingleton<T, _ExplicitSetup, _IsOwning, _IsThreadSafe>::Deleter::operator()( T* a_Ptr ) const
    {
        static_cast<ThisType&>(*a_Ptr).OnSingletonDestroyed();
        delete a_Ptr;
    }

} // namespace Tridium