#pragma once
#include <Tridium/Core/Window.h>

#include <GLFW/glfw3.h>
#include <Tridium/Rendering/RenderingContext.h>

namespace Tridium {

    class WindowsWindow : public Window
    {
    public:
        WindowsWindow( const WindowProps& props );
        virtual ~WindowsWindow();

        void OnUpdate() override;

        uint32_t GetWidth() const override { return m_Data.Width; }
        uint32_t GetHeight() const override { return m_Data.Height; }

        // Window Attributes
        inline void SetEventCallback( const EventCallbackFn& callback ) override { m_Data.EventCallback = callback; }
        void SetVSync( bool enabled ) override;
        bool IsVSync() const override;
        virtual void SetIcon( const std::string& path ) override;

        void* GetNativeWindow() const { return m_Window; }

    private:
        virtual void Init( const WindowProps& props );
        virtual void Shutdown();

    private:
        GLFWwindow* m_Window;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
        RenderingContext* m_RenderingContext;
    };

}