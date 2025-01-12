#pragma once
namespace Tridium {

	class EnvironmentMap;

	struct SceneEnvironment
	{
		struct {
			CubeMapHandle EnvironmentMapHandle;
			SharedPtr<EnvironmentMap> EnvironmentMap;
			float Exposure = 1.0f;
			float Gamma = 2.2f;
			float Blur = 0.0f;
			float Intensity = 1.0f;
			Vector3 RotationEular = { 0.0f, 0.0f, 0.0f };
		} HDRI;
	};
}