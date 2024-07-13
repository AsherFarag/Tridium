#pragma once

namespace Tridium {

    template <typename T>
    class Singleton 
    {
    public:
        static T& Get()
        {
            static T s_Instance;
            static bool s_Initialised = ( s_Instance.Init(), true );
            return s_Instance;
        }

    protected:
        virtual void Init() {}

    private:
        Singleton() = default;
        Singleton( const Singleton& ) = delete;
        Singleton& operator=( const Singleton& ) = delete;
    };

}