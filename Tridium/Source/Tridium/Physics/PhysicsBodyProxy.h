#pragma once
#include "PhysicsScene.h"

namespace Tridium {

	class PhysicsBodyProxy
	{
	public:
		PhysicsBodyProxy()
			: m_PhysicsScene(),
			m_BodyID( INVALID_PHYSICS_BODY_ID )
		{}

		PhysicsBodyProxy( const SharedPtr<PhysicsScene>& a_PhysicsScene, PhysicsBodyID a_BodyID )
			: m_PhysicsScene( a_PhysicsScene )
			, m_BodyID( a_BodyID )
		{}

		~PhysicsBodyProxy()
		{
			if ( !m_PhysicsScene.expired() )
			{
				m_PhysicsScene.lock()->RemovePhysicsBody(m_BodyID);
			}
		}


		WeakPtr<PhysicsScene> GetPhysicsScene() const { return m_PhysicsScene; }
		void SetPhysicsScene( const SharedPtr<PhysicsScene>& a_PhysicsScene ) { m_PhysicsScene = a_PhysicsScene; }
		PhysicsBodyID GetBodyID() const { return m_BodyID; }
		void SetBodyID( PhysicsBodyID a_BodyID ) { m_BodyID = a_BodyID; }

		bool IsValid() const
		{ 
			return !m_PhysicsScene.expired() && m_BodyID != INVALID_PHYSICS_BODY_ID;
		}

		void Destroy()
		{
			if ( !m_PhysicsScene.expired() )
			{
				m_PhysicsScene.lock()->RemovePhysicsBody( m_BodyID );
			}
			m_BodyID = INVALID_PHYSICS_BODY_ID;
		}

		Vector3 GetPosition() const
		{
			if ( !m_PhysicsScene.expired() )
			{
				return m_PhysicsScene.lock()->GetPhysicsBodyPosition( m_BodyID );
			}
			TE_CORE_WARN( "PhysicsBodyProxy::GetPosition - PhysicsScene is expired" );
			return Vector3();
		}

		Quaternion GetRotation() const
		{
			if ( !m_PhysicsScene.expired() )
			{
				return m_PhysicsScene.lock()->GetPhysicsBodyRotation( m_BodyID );
			}
			TE_CORE_WARN( "PhysicsBodyProxy::GetRotation - PhysicsScene is expired" );
			return Quaternion();
		}

		Vector3 GetLinearVelocity() const
		{
			if ( !m_PhysicsScene.expired() )
			{
				return m_PhysicsScene.lock()->GetPhysicsBodyLinearVelocity( m_BodyID );
			}
			TE_CORE_WARN( "PhysicsBodyProxy::GetLinearVelocity - PhysicsScene is expired" );
			return Vector3();
		}

		Vector3 GetAngularVelocity() const
		{
			if ( !m_PhysicsScene.expired() )
			{
				return m_PhysicsScene.lock()->GetPhysicsBodyAngularVelocity( m_BodyID );
			}
			TE_CORE_WARN( "PhysicsBodyProxy::GetAngularVelocity - PhysicsScene is expired" );
			return Vector3();
		}

		bool IsSleeping() const
		{
			if ( !m_PhysicsScene.expired() )
			{
				return m_PhysicsScene.lock()->IsPhysicsBodySleeping( m_BodyID );
			}
			TE_CORE_WARN( "PhysicsBodyProxy::IsSleeping - PhysicsScene is expired" );
			return false;
		}

		void SetPosition( const Vector3& a_Position )
		{
			if ( !m_PhysicsScene.expired() )
				m_PhysicsScene.lock()->SetPhysicsBodyPosition( m_BodyID, a_Position );
			else
				TE_CORE_WARN( "PhysicsBodyProxy::SetPosition - PhysicsScene is expired" );
		}

		void SetRotation( const Quaternion& a_Rotation )
		{
			if ( !m_PhysicsScene.expired() )
				m_PhysicsScene.lock()->SetPhysicsBodyRotation( m_BodyID, a_Rotation );
			else
				TE_CORE_WARN( "PhysicsBodyProxy::SetRotation - PhysicsScene is expired" );
		}

		void SetFriction( float a_Friction )
		{
			if ( !m_PhysicsScene.expired() )
				m_PhysicsScene.lock()->SetPhysicsBodyFriction( m_BodyID, a_Friction );
			else
				TE_CORE_WARN( "PhysicsBodyProxy::SetFriction - PhysicsScene is expired" );
		}

		void SetLinearVelocity( const Vector3& a_LinearVelocity )
		{
			if ( !m_PhysicsScene.expired() )
				m_PhysicsScene.lock()->SetPhysicsBodyLinearVelocity( m_BodyID, a_LinearVelocity );
			else
				TE_CORE_WARN( "PhysicsBodyProxy::SetLinearVelocity - PhysicsScene is expired" );
		}

		void SetAngularVelocity( const Vector3& a_AngularVelocity )
		{
			if ( !m_PhysicsScene.expired() )
				m_PhysicsScene.lock()->SetPhysicsBodyAngularVelocity( m_BodyID, a_AngularVelocity );
			else
				TE_CORE_WARN( "PhysicsBodyProxy::SetAngularVelocity - PhysicsScene is expired" );
		}

		void AddImpulse( const Vector3& a_Impulse )
		{
			if ( !m_PhysicsScene.expired() )
				m_PhysicsScene.lock()->AddImpulseToPhysicsBody( m_BodyID, a_Impulse );
			else
				TE_CORE_WARN( "PhysicsBodyProxy::AddImpulse - PhysicsScene is expired" );
		}

		void AddImpulse( const Vector3& a_Impulse, const Vector3& a_Position )
		{
			if ( !m_PhysicsScene.expired() )
				m_PhysicsScene.lock()->AddImpulseToPhysicsBody( m_BodyID, a_Impulse, a_Position );
			else
				TE_CORE_WARN( "PhysicsBodyProxy::AddImpulse - PhysicsScene is expired" );
		}

	private:
		WeakPtr<PhysicsScene> m_PhysicsScene;
		PhysicsBodyID m_BodyID;
	};

}