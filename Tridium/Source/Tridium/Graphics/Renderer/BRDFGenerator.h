#pragma once
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include <Tridium/Containers/Array.h>
#include <cmath>

namespace Tridium {

	// Utility function for generating a BRDF-LUT(Bidirectional Reflectance Distribution Function Look-Up Table).
	static Array<Pair<float, float>> GenerateBRDFLUTData( const size_t _Size ) noexcept
    {
        Array<Pair<float, float>> data{};
		data.Resize( _Size * _Size );

        auto RadicalInverse_VdC = []( uint32_t bits ) -> float
        {
            bits = ( bits << 16u ) | ( bits >> 16u );
            bits = ( ( bits & 0x55555555u ) << 1u ) | ( ( bits & 0xAAAAAAAAu ) >> 1u );
            bits = ( ( bits & 0x33333333u ) << 2u ) | ( ( bits & 0xCCCCCCCCu ) >> 2u );
            bits = ( ( bits & 0x0F0F0F0Fu ) << 4u ) | ( ( bits & 0xF0F0F0F0u ) >> 4u );
            bits = ( ( bits & 0x00FF00FFu ) << 8u ) | ( ( bits & 0xFF00FF00u ) >> 8u );
            return float( bits ) * 2.3283064365386963e-10f; // / 0x100000000
        };

        auto Hammersley = [&]( uint32_t i, uint32_t N ) -> Pair<float, float>
        {
            return { float( i ) / float( N ), RadicalInverse_VdC( i ) };
        };

        auto ImportanceSampleGGX = []( Pair<float, float> Xi, float roughness ) -> FixedArray<float, 3>
        {
            float a = roughness * roughness;

            float phi = 2.0f * 3.14159265358979323846f * Xi.second;
            float cosTheta = std::sqrt( ( 1.0f - Xi.first ) / ( 1.0f + ( a * a - 1.0f ) * Xi.first ) );
            float sinTheta = std::sqrt( std::max( 0.0f, 1.0f - cosTheta * cosTheta ) );

            return { sinTheta * std::cos( phi ), sinTheta * std::sin( phi ), cosTheta };
        };

        auto GeometrySchlickGGX = []( float NdotV, float roughness ) -> float
        {
            float a = roughness;
            float k = ( a * a ) / 2.0f;
            return NdotV / ( NdotV * ( 1.0f - k ) + k );
        };

        auto GeometrySmith = [&]( float NdotV, float NdotL, float roughness ) -> float
        {
            float ggx1 = GeometrySchlickGGX( NdotV, roughness );
            float ggx2 = GeometrySchlickGGX( NdotL, roughness );
            return ggx1 * ggx2;
        };

        constexpr uint32_t NumSamples = 16;

        for ( size_t y = 0; y < _Size; ++y )
        {
            float roughness = ( y + 0.5f ) / float( _Size );

            for ( size_t x = 0; x < _Size; ++x )
            {
                float NdotV = ( x + 0.5f ) / float( _Size );
                FixedArray<float, 3> V = { std::sqrt( 1.0f - NdotV * NdotV ), 0.0f, NdotV };

                float A = 0.0f;
                float B = 0.0f;

                for ( uint32_t i = 0; i < NumSamples; ++i )
                {
                    auto Xi = Hammersley( i, NumSamples );
                    auto H = ImportanceSampleGGX( Xi, roughness );

                    float dotVH = std::max( 0.0f, V[0] * H[0] + V[1] * H[1] + V[2] * H[2] );
                    FixedArray<float, 3> L = {
                        2.0f * dotVH * H[0] - V[0],
                        2.0f * dotVH * H[1] - V[1],
                        2.0f * dotVH * H[2] - V[2]
                    };

                    float NdotL = std::max( L[2], 0.0f );
                    float NdotH = std::max( H[2], 0.0f );
                    float VdotH = std::max( dotVH, 0.0f );

                    if ( NdotL > 0.0f )
                    {
                        float G = GeometrySmith( NdotV, NdotL, roughness );
                        float G_Vis = ( G * VdotH ) / ( NdotH * NdotV );
                        float Fc = std::pow( 1.0f - VdotH, 5.0f );

                        A += ( 1.0f - Fc ) * G_Vis;
                        B += Fc * G_Vis;
                    }
                }

                A /= float( NumSamples );
                B /= float( NumSamples );

				data[y * _Size + x] = { A, B };
            }
        }

		return data;
    }

}