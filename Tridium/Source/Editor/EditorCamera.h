#pragma once
#ifdef IS_EDITOR

#include <Tridium/Rendering/Camera.h>

namespace Tridium::Editor {

    class EditorCamera final : public Camera
    {
    public:
        EditorCamera();

        void OnUpdate();

        void LerpTo( const Vector3& pos );

        Vector3 GetUpDirection() const;
        Vector3 GetRightDirection() const;
        Vector3 GetForwardDirection() const;
        Quaternion GetOrientation() const;
        Matrix4 GetTransform() const;
        const Matrix4& GetViewMatrix() const { return m_View; }

        bool Focused = true;

        float Sensitivity = 0.01f;
        float Speed = 3.f;

        Vector3 Position = Vector3( 0 );
        float Pitch = 0.f, Yaw = 0.f;
        Vector3 Scale = Vector3( 1 );

    private:
        void HandleInput();

        void MoveForward( const float magnitude, const float speed );
        void MoveSideways( const float magnitude, const float speed );
        void MouseRotate( const Vector2& mouseDelta );

        void RecalculateView();

        void Lerp();

    private:
        Matrix4 m_View;
        Vector2 m_LastMousePos;

        bool m_IsMoving = false;
        float m_MaxTimeMoving = 10.f;
        float m_TimeMoving = 0.0f;
        float m_MaxTimeMovingSpeedMultiplier = 30.f;
        float m_TimeMovingSpeedMultiplier = 1.0f;

        struct
        {
            float LerpTime = 0.5f;
            float CurrLerpTime = 0.0f;
            Vector3 LerpToPos = {};
            bool IsLerping = false;
        } m_LerpData;
    };

}
#endif // IS_EDITOR