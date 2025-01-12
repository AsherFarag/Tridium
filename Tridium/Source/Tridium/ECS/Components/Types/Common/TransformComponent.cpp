#include "tripch.h"
#include "TransformComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( TransformComponent, Scriptable )
		OVERRIDE( Props::RegisterScriptableProp, +[]( Script::ScriptEngine& a_ScriptEngine )
			{
				auto type = a_ScriptEngine.RegisterNewType<TransformComponent>( "TransformComponent" );
				type["gameObject"] = sol::property( &TransformComponent::GetGameObject );
				type["GetLocalPosition"] = &TransformComponent::GetLocalPosition;
				type["GetWorldPosition"] = &TransformComponent::GetWorldPosition;
				type["SetLocalPosition"] = &TransformComponent::SetLocalPosition;
				type["SetWorldPosition"] = &TransformComponent::SetWorldPosition;
			} )
		BASE( Component )
		PROPERTY( Position, Serialize | EditAnywhere )
		PROPERTY( Rotation, Serialize | EditAnywhere )
		PROPERTY( Scale, Serialize | EditAnywhere )
		PROPERTY( m_Parent, Serialize )
		PROPERTY( m_Children, Serialize )
	END_REFLECT_COMPONENT( TransformComponent )

	TransformComponent::TransformComponent( const Vector3& a_Translation )
		: Position( a_Translation ) {}

	void TransformComponent::OnEndPlay()
	{
		DetachFromParent();

		for ( auto child : m_Children )
		{
			DetachChild( child );
		}
	}

	Matrix4 TransformComponent::GetWorldTransform() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetWorldTransform() * GetLocalTransform();
		else
			return GetLocalTransform();
	}

	Matrix4 TransformComponent::GetLocalTransform() const
	{
		Matrix4 rotationMatrix = glm::toMat4( Rotation.Quat );

		constexpr Matrix4 identity = Matrix4( 1.0f );

		return glm::translate( identity, Position )
			* rotationMatrix
			* glm::scale( identity, Scale );
	}

	Vector3 TransformComponent::GetForward() const
	{
		Vector3 forward = Rotation.Quat * Vector3( 0.0f, 0.0f, 1.0f );

		if ( m_Parent.IsValid() )
		{
			forward += m_Parent.GetTransform().GetForward();
		}

		return glm::normalize(forward);
	}

	Vector3 TransformComponent::GetRight() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetRight() * Rotation.Quat * Vector3( 1.0f, 0.0f, 0.0f );
		else
			return Rotation.Quat * Vector3( 1.0f, 0.0f, 0.0f );
	}

	Vector3 TransformComponent::GetUp() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetUp() * Rotation.Quat * Vector3( 0.0f, 1.0f, 0.0f );
		else
			return Rotation.Quat * Vector3( 0.0f, 1.0f, 0.0f );
	}

	Quaternion TransformComponent::GetOrientation() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetOrientation() * Rotation.Quat;
		else
			return Rotation.Quat;
	}

	Vector3 TransformComponent::GetWorldScale() const
	{
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetWorldScale() * Scale;
		else
			return Scale;
	}

	void TransformComponent::SetWorldPosition( const Vector3& a_Position )
	{
		if ( m_Parent.IsValid() )
		{
			const Matrix4 parentTransform = glm::inverse( m_Parent.GetTransform().GetWorldTransform() );
			Position = parentTransform * glm::vec4( a_Position, 1.0f );
		}
		else
		{
			Position = a_Position;
		}
	}

	void TransformComponent::SetLocalPosition( const Vector3& a_Position )
	{
		Position = a_Position;
	}

	void TransformComponent::SetWorldScale( const Vector3& a_Scale )
	{
		if ( m_Parent.IsValid() )
			Scale = a_Scale / m_Parent.GetTransform().GetWorldScale();
		else
			Scale = a_Scale;
	}

	void TransformComponent::SetLocalScale( const Vector3& a_Scale )
	{
		Scale = a_Scale;
	}

	void TransformComponent::SetWorldRotation( const Quaternion& a_Rotation )
	{
		if ( m_Parent.IsValid() )
			Rotation = a_Rotation * glm::inverse( m_Parent.GetTransform().GetOrientation() );
		else
			Rotation = a_Rotation;
	}

	void TransformComponent::SetLocalRotation( const Quaternion& a_Rotation )
	{
		Rotation = a_Rotation;
	}

	void TransformComponent::SetWorldTransform( const Matrix4& a_Transform )
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

	void TransformComponent::SetLocalTransform( const Matrix4& a_Transform )
	{
		Quaternion rotation;
		Math::DecomposeTransform( a_Transform, Position, rotation, Scale );
		Rotation.SetFromQuaternion( rotation );
	}

	void TransformComponent::AttachToParent( GameObject a_Parent )
	{
		a_Parent.GetTransform().AttachChild(GetGameObject());
	}

	void TransformComponent::DetachFromParent()
	{
		if ( HasParent() )
			GetParent().GetTransform().DetachChild(GetGameObject());
	}

	void TransformComponent::AttachChild( GameObject a_Child )
	{
		TransformComponent& childTransform = a_Child.GetTransform();
		if ( childTransform.GetParent() != GetGameObject() && a_Child != GetParent() )
		{
			childTransform.DetachFromParent();
			childTransform.SetParent( GetGameObject() );
			m_Children.push_back( a_Child );
		}
	}

	void TransformComponent::DetachChild( GameObject a_Child )
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

	GameObject TransformComponent::GetChild( const std::string& a_Tag ) const
	{
		for ( GameObject child : m_Children )
		{
			if ( child.GetTag() == a_Tag )
			{
				return child;
			}
		}

		return GameObject();
	}

	void TransformComponent::RemoveChild( GameObject a_Child )
	{
		m_Children.erase( std::find( m_Children.begin(), m_Children.end(), a_Child ) );
	}
}
