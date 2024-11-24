#include "tripch.h"
#include "TransformComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( TransformComponent )
		BASE( Component )
		PROPERTY( Position, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( Rotation, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( Scale, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_Parent, FLAGS( Serialize ) )
		PROPERTY( m_Children, FLAGS( Serialize ) )
	END_REFLECT( TransformComponent )

	TransformComponent::TransformComponent( const Vector3& a_Translation )
		: Position( a_Translation ) {}

	void TransformComponent::OnDestroy()
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
		if ( m_Parent.IsValid() )
			return m_Parent.GetTransform().GetForward() * Rotation.Quat * Vector3( 0.0f, 0.0f, 1.0f );
		else
			return Rotation.Quat * Vector3( 0.0f, 0.0f, 1.0f );
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
