#include "tripch.h"
#include "TransformComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Tridium {

	START_REFLECT( TransformComponent )
		PROPERTY( TransformComponent, Position )
		PROPERTY( TransformComponent, Rotation )
		PROPERTY( TransformComponent, Scale )
		FUNCTION( TransformComponent, GetForward )
	END_REFLECT

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
		Matrix4 rotationMatrix = glm::toMat4( Rotation );

		constexpr Matrix4 identity = Matrix4( 1.0f );

		return glm::translate( identity, Position )
			* rotationMatrix
			* glm::scale( identity, Scale );
	}

	Vector3 TransformComponent::GetForward() const
	{
		return glm::rotate( GetOrientation(), Vector3( 0.0f, 0.0f, -1.0f ) );
	}

	Quaternion TransformComponent::GetOrientation() const
	{
		return Quaternion( Vector3( -Rotation.x, -Rotation.y, 0.f ) );
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
		auto& childTransform = a_Child.GetTransform();
		if ( childTransform.GetParent() != GetGameObject() && a_Child != GetParent() )
		{
			childTransform.DetachFromParent();

			// We want to keep the child's world transform, so we need to change the childs local transform
			Matrix4 newLocalTransform = glm::inverse( GetWorldTransform() ) * a_Child.GetWorldTransform();
			TransformComponent& tc = a_Child.GetTransform();
			Math::DecomposeTransform( newLocalTransform, tc.Position, tc.Rotation, tc.Scale );

			childTransform.SetParent( GetGameObject() );
			m_Children.push_back( a_Child );
		}
	}

	void TransformComponent::DetachChild( GameObject a_Child )
	{
		auto& childTransform = a_Child.GetTransform();
		if ( childTransform.GetParent() == GetGameObject() )
		{
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
