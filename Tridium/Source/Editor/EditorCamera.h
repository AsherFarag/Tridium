#pragma once
#ifdef IS_EDITOR

#include <Tridium/Rendering/Camera.h>

namespace Tridium::Editor {

    class EditorCamera final : public Camera
    {
    public:

        void OnUpdate();

        Vector3 GetUpDirection() const;
        Vector3 GetRightDirection() const;
        Vector3 GetForwardDirection() const;
        Quaternion GetOrientation() const;

        const Matrix4& GetViewMatrix() const { return m_View; }

    public:
        bool Focused = true;

        float Sensitivity = 0.01f;
        float Speed = 3.f;

        Vector3 Position = Vector3( 0 );
        float Pitch = 0.f, Yaw = 0.f;
        Vector3 Scale = Vector3( 1 );

    private:
        void HandleInput();

        void MoveForward( const float magnitude );
        void MoveSideways( const float magnitude );
        void MouseRotate( const Vector2& mouseDelta );

        void RecalculateView();

    private:
        Matrix4 m_View;
        Vector2 m_LastMousePos;

        bool m_WasLeftShiftPressed = false;
        bool m_WasLeftCtrlPressed = false;
    };

}
#endif // IS_EDITOR