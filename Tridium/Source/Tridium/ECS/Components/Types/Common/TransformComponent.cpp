#include "tripch.h"
#include "TransformComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( OldTransformComponent, Scriptable )
		OVERRIDE( Props::RegisterScriptableProp, +[]( ScriptEngine& a_ScriptEngine )
			{
				auto type = a_ScriptEngine.RegisterNewType<OldTransformComponent>( "TransformComponent" );
				type["gameObject"] = sol::property( &OldTransformComponent::GetGameObject );
				type["GetLocalPosition"] = &OldTransformComponent::GetLocalPosition;
				type["GetWorldPosition"] = &OldTransformComponent::GetWorldPosition;
				type["SetLocalPosition"] = &OldTransformComponent::SetLocalPosition;
				type["SetWorldPosition"] = &OldTransformComponent::SetWorldPosition;
			} )
		BASE( Component )
		PROPERTY( Position, Serialize | EditAnywhere )
		PROPERTY( Rotation, Serialize | EditAnywhere )
		PROPERTY( Scale, Serialize | EditAnywhere )
		PROPERTY( m_Parent, Serialize )
		PROPERTY( m_Children, Serialize )
	END_REFLECT_COMPONENT( OldTransformComponent )

	OldTransformComponent::OldTransformComponent( const Vector3& a_Translation )
		: Position( a_Translation ) {}

	void OldTransformComponent::OnEndPlay()
	{
		DetachFromParent();

		for ( auto child : m_Children )
		{
			DetachChild( child );
		}
	}

	Matrix4 OldTransformComponent::GetWorldTransform() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetWorldTransform() * GetLocalTransform();
		else
			return GetLocalTransform();
	}

	Matrix4 OldTransformComponent::GetLocalTransform() const
	{
		Matrix4 rotationMatrix = Math::ToMat4( Rotation.Quat );

		constexpr Matrix4 identity = Matrix4( 1.0f );

		return Math::Translate( identity, Position )
			* rotationMatrix
			* Math::Scale( identity, Scale );
	}

	Vector3 OldTransformComponent::GetForward() const
	{
		Vector3 forward = Rotation.Quat * Vector3( 0.0f, 0.0f, 1.0f );

		if ( m_Parent.IsValid() )
		{
			forward += m_Parent.GetTransform().GetForward();
		}

		return forward.Normalized();
	}

	Vector3 OldTransformComponent::GetRight() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetRight() * Rotation.Quat * Vector3( 1.0f, 0.0f, 0.0f );
		else
			return Rotation.Quat * Vector3( 1.0f, 0.0f, 0.0f );
	}

	Vector3 OldTransformComponent::GetUp() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetUp() * Rotation.Quat * Vector3( 0.0f, 1.0f, 0.0f );
		else
			return Rotation.Quat * Vector3( 0.0f, 1.0f, 0.0f );
	}

	Quaternion OldTransformComponent::GetOrientation() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetOrientation() * Rotation.Quat;
		else
			return Rotation.Quat;
	}

	Vector3 OldTransformComponent::GetWorldScale() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetWorldScale() * Scale;
		else
			return Scale;
	}

	void OldTransformComponent::SetWorldPosition( const Vector3& a_Position )
	{
		if ( m_Parent.IsValid() )
		{
			const Matrix4 parentTransform = glm::inverse( m_Parent.GetTransform().GetWorldTransform() );
			Position = parentTransform * Vector4( a_Position, 1.0f );
		}
		else
		{
			Position = a_Position;
		}
	}

	void OldTransformComponent::SetLocalPosition( const Vector3& a_Position )
	{
		Position = a_Position;
	}

	void OldTransformComponent::SetWorldScale( const Vector3& a_Scale )
	{
		if ( m_Parent.IsValid() )
			Scale = a_Scale / m_Parent.GetTransform().GetWorldScale();
		else
			Scale = a_Scale;
	}

	void OldTransformComponent::SetLocalScale( const Vector3& a_Scale )
	{
		Scale = a_Scale;
	}

	void OldTransformComponent::SetWorldRotation( const Quaternion& a_Rotation )
	{
		if ( m_Parent.IsValid() )
			Rotation = a_Rotation * glm::inverse( m_Parent.GetTransform().GetOrientation() );
		else
			Rotation = a_Rotation;
	}

	void OldTransformComponent::SetLocalRotation( const Quaternion& a_Rotation )
	{
		Rotation = a_Rotation;
	}

	void OldTransformComponent::SetWorldTransform( const Matrix4& a_Transform )
	{
		if ( m_Parent.IsValid() )
		{
			Matrix4 parentTransform = m_Parent.GetTransform().GetWorldTransform();
			Matrix4 parentTransformInverse = glm::inverse( parentTransform );
			Matrix4 localTransform = parentTransformInverse * a_Transform;

			Quaternion rotation;
			Math::DecomposeTransform( localTransform, Position, rotation, Scale );
			Rotation.SetFromQuaternion( rotation );
		}
		else
		{
			Quaternion rotation;
			Math::DecomposeTransform( a_Transform, Position, rotation, Scale );
			Rotation.SetFromQuaternion( rotation );
		}
	}

	void OldTransformComponent::SetLocalTransform( const Matrix4& a_Transform )
	{
		Quaternion rotation;
		Math::DecomposeTransform( a_Transform, Position, rotation, Scale );
		Rotation.SetFromQuaternion( rotation );
	}

	void OldTransformComponent::AttachToParent( OldGameObject a_Parent )
	{
		a_Parent.GetTransform().AttachChild(GetGameObject());
	}

	void OldTransformComponent::DetachFromParent()
	{
		if ( HasParent() )
			GetParent().GetTransform().DetachChild(GetGameObject());
	}

	void OldTransformComponent::AttachChild( OldGameObject a_Child )
	{
		OldTransformComponent& childTransform = a_Child.GetTransform();
		if ( childTransform.GetParent() != GetGameObject() && a_Child != GetParent() )
		{
			childTransform.DetachFromParent();
			childTransform.SetParent( GetGameObject() );
			m_Children.push_back( a_Child );
		}
	}

	void OldTransformComponent::DetachChild( OldGameObject a_Child )
	{
		auto& childTransform = a_Child.GetTransform();
		if ( childTransform.GetParent() == GetGameObject() )
		{
			Quaternion rotation = childTransform.Rotation.Quat;
			Math::DecomposeTransform( childTransform.GetWorldTransform(), childTransform.Position, rotation, childTransform.Scale);
			childTransform.Rotation.SetFromQuaternion( rotation );

			childTransform.SetParent();
			RemoveChild( a_Child );
		}
	}

	OldGameObject OldTransformComponent::GetChild( const std::string& a_Tag ) const
	{
		for ( OldGameObject child : m_Children )
		{
			if ( child.GetTag() == a_Tag )
			{
				return child;
			}
		}

		return OldGameObject();
	}

	void OldTransformComponent::RemoveChild( OldGameObject a_Child )
	{
		m_Children.erase( std::find( m_Children.begin(), m_Children.end(), a_Child ) );
	}
}
