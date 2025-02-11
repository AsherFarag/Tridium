#include "tripch.h"
#include "Math.h"
#include "ImGuizmo.h"

namespace Tridium::Math {
#if 0

	void DecomposeTransform( const Matrix4& mat, Vector3& outPos, Vector3& outRot, Vector3& outScale )
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix( mat );

		// Normalize the matrix.
		if ( epsilonEqual( LocalMatrix[3][3], static_cast<float>( 0 ), epsilon<T>() ) )
			return;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual( LocalMatrix[0][3], static_cast<T>( 0 ), epsilon<T>() ) ||
			epsilonNotEqual( LocalMatrix[1][3], static_cast<T>( 0 ), epsilon<T>() ) ||
			epsilonNotEqual( LocalMatrix[2][3], static_cast<T>( 0 ), epsilon<T>() ) )
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>( 0 );
			LocalMatrix[3][3] = static_cast<T>( 1 );
		}

		// Next take care of translation (easy).
		outPos = vec3( LocalMatrix[3] );
		LocalMatrix[3] = vec4( 0, 0, 0, LocalMatrix[3].w );

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for ( length_t i = 0; i < 3; ++i )
			for ( length_t j = 0; j < 3; ++j )
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		outScale.x = length( Row[0] );
		Row[0] = detail::scale( Row[0], static_cast<T>( 1 ) );
		outScale.y = length( Row[1] );
		Row[1] = detail::scale( Row[1], static_cast<T>( 1 ) );
		outScale.z = length( Row[2] );
		Row[2] = detail::scale( Row[2], static_cast<T>( 1 ) );

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross( Row[1], Row[2] ); // v3Cross(row[1], row[2], Pdum3);
		if ( dot( Row[0], Pdum3 ) < 0 )
		{
			for ( length_t i = 0; i < 3; i++ )
			{
				scale[i] *= static_cast<T>( -1 );
				Row[i] *= static_cast<T>( -1 );
			}
		}
#endif

		outRot.y = asin( -Row[0][2] );
		if ( cos( outRot.y ) != 0 ) {
			outRot.x = atan2( Row[1][2], Row[2][2] );
			outRot.z = atan2( Row[0][1], Row[0][0] );
		}
		else {
			outRot.x = atan2( -Row[2][0], Row[1][1] );
			outRot.z = 0;
		}
	}

	void DecomposeTransform( const Matrix4& mat, Vector3& outPos, Quaternion& outRot, Vector3& outScale )
	{
		Vector3 tempRot = {};
		DecomposeTransform( mat, outPos, tempRot, outScale );
		outRot = Quaternion( tempRot );
	}
#endif
}