#pragma once
#include <Tridium/Scene/Component.h>
#include <Tridium/Asset/Asset.h>
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Asset/EnvironmentMapAsset.h>
#include <Tridium/Graphics/Color.h>

namespace Tridium {

	enum class EAspectRatioMode : uint8_t
	{
		// No constraint — freely resizes with the window.
		Free = 0,

		// Keeps a fixed aspect ratio (e.g., 16:9 or 4:3), adding letterboxing or pillarboxing as needed.
		Fixed,

		// Matches the width, adjusting height to maintain content scaling.
		MatchWidth,

		// Matches the height, adjusting width to maintain content scaling.
		MatchHeight,

		// Expands to fill the entire viewport, possibly distorting the image.
		Stretch,

		// Maintains aspect ratio but crops content to fill the viewport.
		Crop,

		// Scales uniformly to fit the viewport within bounds (no cropping, black bars possible).
		Fit,

		// Scales uniformly to fill the viewport entirely (cropping possible).
		Fill,
	};

	//=================================================================================================
	// Camera Component: Defines a camera in the scene.
	//=================================================================================================
	struct CameraComponent : Component
	{
		enum class EProjectionType { Perspective = 0, Orthographic = 1 };

		EProjectionType ProjectionType = EProjectionType::Perspective;
		Vector2 ViewportSize{ 1280, 720 };

		struct PerspectiveData
		{
			float NearPlane = 0.01f;
			float FarPlane = 1000.0f;
			float FOV = 45.0f;
		} Perspective;

		struct OrthographicData
		{
			float NearPlane = -1000.0f;
			float FarPlane = 1000.0f;
			float Size = 10.0f;
		} Orthographic;

		float NearPlane() const { return ProjectionType == EProjectionType::Perspective ? Perspective.NearPlane : Orthographic.NearPlane; }
		float FarPlane() const { return ProjectionType == EProjectionType::Perspective ? Perspective.FarPlane : Orthographic.FarPlane; }
		float AspectRatio() const { return ViewportSize.X / ViewportSize.Y; }
		Matrix4 CalculatePerspective() const;
		Matrix4 CalculateOrthographic() const;
		Matrix4 CalculateProjection() const { return ProjectionType == EProjectionType::Perspective ? CalculatePerspective() : CalculateOrthographic(); }
	};

	//=================================================================================================
	// Static Mesh Component: Renders a static mesh in the scene.
	//=================================================================================================
	struct StaticMeshComponent : Component
	{
		AssetHandle<StaticMesh> Mesh;
		bool CastShadows = true;
	};

	//=================================================================================================
	// Directional Light Component: A light that has a direction but no specific position.
	//=================================================================================================
	struct DirectionalLightComponent : Component
	{
		// Whether the light is used for rendering.
		bool Enabled = true;

		// The color of the light, represented as RGB values.
		Color3 Color{ 1.0f, 1.0f, 1.0f };

		// The intensity of the light, affecting its brightness.
		float Intensity{ 1.0f };

		// A scale factor for specular highlights produced by the light.
		// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
		float SpecularScale{ 1.0f };

		// Whether the light casts shadows in the scene.
		bool CastShadows = true;

		// The resolution of the shadow map texture used for rendering shadows from this light.
		Vector2 ShadowMapResolution{ 2048, 2048 };

		// The shadow map texture used for rendering shadows from this light.
		RHITextureRef ShadowMap;
	};

	//=================================================================================================
	// Point Light Component: 
	// A light that emits light uniformly in all directions from a single point in space.
	//=================================================================================================
	struct PointLightComponent : Component
	{
		// Whether the light is used for rendering.
		bool Enabled = true;

		// The color of the light, represented as RGB values.
		Color3 Color{ 1.0f, 1.0f, 1.0f };

		// The intensity of the light, affecting its brightness.
		float Intensity = 1.0f;

		// The minimum radius for light attenuation calculations.
		float MinRadius = 0.001f;

		// The maximum radius for light attenuation calculations.
		float Radius = 5.0f;

		// The falloff exponent for light attenuation, controlling how quickly the light diminishes with distance.
		float Falloff = 1.f;

		// The size of the light source, affecting soft shadow calculations.
		float SourceSize = 0.001f;

		// A scale factor for specular highlights produced by the light.
		// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
		float SpecularScale = 1.0f;

		// Whether the light casts shadows in the scene.
		bool CastShadows = true;
	};

	//=================================================================================================
	// Spot Light Component: A light that emits light in a specific direction within a cone,
	// simulating a flashlight or spotlight.
	//=================================================================================================
	struct SpotLightComponent : Component
	{
		// Whether the light is used for rendering.
		bool Enabled = true;

		// The color of the light, represented as RGB values.
		Color3 Color{ 1.0f, 1.0f, 1.0f };

		// The intensity of the light, affecting its brightness.
		float Intensity = 1.0f;

		// The minimum radius for light attenuation calculations.
		float Range = 10.0f;

		// The angle (in degrees) defining the spotlight's bright center.
		float InnerConeAngleDeg = 30.0f;

		// The angle (in degrees) defining the spotlight's outer edge where light falls off to zero.
		float OuterConeAngleDeg = 45.0f;

		// The falloff exponent for light attenuation, controlling how quickly the light diminishes with distance.
		float Falloff = 1.0f;

		// The size of the light source, affecting soft shadow calculations.
		float SourceSize = 0.001f;

		// A scale factor for specular highlights produced by the light.
		// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
		float SpecularScale = 1.0f;

		// Whether the light casts shadows in the scene.
		bool CastShadows = true;
	};

	//=================================================================================================
	// Skybox Component: Renders a skybox in the scene using an environment map.
	//=================================================================================================
	struct SkyboxComponent : Component
	{
		AssetHandle<EnvironmentMap> EnvironmentMap;
		float Exposure = 1.0f;
		float Gamma = 2.2f;
		float Blur = 0.0f;
		float Intensity = 1.0f;
	};

} // namespace Tridium

namespace Tridium::Meta {

	template<>
	struct Reflector<CameraComponent>
	{
		using Type = Type<CameraComponent, Scriptable, Icon<EditorIcons::CameraRetro>>;

		Field<&CameraComponent::ProjectionType, Serializable, Editable, Scriptable,
		Tooltip<"The projection type of the camera.">>
		ProjectionType;

		Field<&CameraComponent::ViewportSize, Serializable, Editable, Scriptable, Min<1.0f>,
		Tooltip<"The size of the camera viewport in pixels.">>
		ViewportSize;

		Field<&CameraComponent::Orthographic, Serializable, Editable, Scriptable,
		DisplayIf<[]( const CameraComponent& a_Camera ) { return a_Camera.ProjectionType == CameraComponent::EProjectionType::Orthographic; }>,
		Tooltip<"The orthographic projection settings.">>
		Orthographic;

		Field<&CameraComponent::Perspective, Serializable, Editable, Scriptable,
		DisplayIf<[]( const CameraComponent& a_Camera ) { return a_Camera.ProjectionType == CameraComponent::EProjectionType::Perspective; }>,
		Tooltip<"The perspective projection settings.">>
		Perspective;

		Function<&CameraComponent::CalculateProjection, Scriptable>
		CalculateProjection;
	};

	template<>
	struct Reflector<CameraComponent::EProjectionType>
	{
		using Type = Type<CameraComponent::EProjectionType, Scriptable>;

		Constant<CameraComponent::EProjectionType::Perspective, Scriptable>
		Perspective;
		
		Constant<CameraComponent::EProjectionType::Orthographic, Scriptable>
		Orthographic;
	};

	template<>
	struct Reflector<CameraComponent::PerspectiveData>
	{
		using Type = Type<CameraComponent::PerspectiveData, Scriptable>;

		Field<&CameraComponent::PerspectiveData::NearPlane, Serializable, Editable, Scriptable,
		Tooltip<"The near clipping plane distance.">>
		NearPlane;

		Field<&CameraComponent::PerspectiveData::FarPlane, Serializable, Editable, Scriptable,
		Tooltip<"The far clipping plane distance.">>
		FarPlane;

		Field<&CameraComponent::PerspectiveData::FOV, Serializable, Editable, Scriptable,
		Tooltip<"The field of view in degrees.">>
		FOV;
	};

	template<>
	struct Reflector<CameraComponent::OrthographicData>
	{
		using Type = Type<CameraComponent::OrthographicData, Scriptable>;

		Field<&CameraComponent::OrthographicData::NearPlane, Serializable, Editable, Scriptable,
		Tooltip<"The near clipping plane distance.">>
		NearPlane;

		Field<&CameraComponent::OrthographicData::FarPlane, Serializable, Editable, Scriptable,
		Tooltip<"The far clipping plane distance.">>
		FarPlane;

		Field<&CameraComponent::OrthographicData::Size, Serializable, Editable, Scriptable,
		Tooltip<"The size of the orthographic view.">>
		Size;
	};

	template<>
	struct Reflector<StaticMeshComponent>
	{
		using Type = Type<StaticMeshComponent, Scriptable, Icon<EditorIcons::Shapes>>;

		Field<&StaticMeshComponent::Mesh, Serializable, Editable, Scriptable,
		Tooltip<"The static mesh asset to render.">>
		Mesh;

		Field<&StaticMeshComponent::CastShadows, Serializable, Editable, Scriptable,
		Tooltip<"Whether this mesh should cast shadows.">>
		CastShadows;
	};

	template<>
	struct Reflector<DirectionalLightComponent>
	{
		using Type = Type<DirectionalLightComponent, Scriptable, Icon<EditorIcons::Sun>>;

		Field<&DirectionalLightComponent::Enabled, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light is enabled.">>
		Enabled;

		Field<&DirectionalLightComponent::Color, Serializable, Editable, Scriptable,
		Tooltip<"The color of the light.">>
		Color;

		Field<&DirectionalLightComponent::Intensity, Serializable, Editable, Scriptable,
		Min<0.0f>,
		Tooltip<"The intensity of the light. Color * Intensity defines the final color of the light.">>
		Intensity;

		Field<&DirectionalLightComponent::SpecularScale, Serializable, Editable, Scriptable,
		Min<0.0f>,
		Tooltip<"Multiplier for specular highlights produced by the light. Any value other than 1.0 breaks energy conservation.">>
		SpecularScale;

		Header Shadow;

		Field<&DirectionalLightComponent::CastShadows, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light casts shadows.">>
		CastShadows;

		Field<&DirectionalLightComponent::ShadowMapResolution, Serializable, Editable, Scriptable,
		DisplayIf<[]( const DirectionalLightComponent& a_Light ) { return a_Light.CastShadows; }>,
		Tooltip<"The resolution of the shadow map texture used for rendering shadows from this light." >>
		ShadowMapResolution;

		Field<&DirectionalLightComponent::ShadowMap, Editable, Scriptable,
		DisplayIf<[]( const DirectionalLightComponent& a_Light ) { return a_Light.CastShadows; } >,
		Tooltip<"The shadow map texture used for rendering shadows from this light." >>
		ShadowMap;
	};

	template<>
	struct Reflector<PointLightComponent>
	{
		using Type = Type<PointLightComponent, Scriptable, Icon<EditorIcons::Lightbulb>>;

		Field<&PointLightComponent::Enabled, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light is enabled.">>
		Enabled;

		Field<&PointLightComponent::Color, Serializable, Editable, Scriptable,
		Tooltip<"The color of the light.">>
		Color;

		Field<&PointLightComponent::Intensity, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The intensity of the light. Color * Intensity defines the final color of the light.">>
		Intensity;

		Field<&PointLightComponent::MinRadius, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The minimum radius for light attenuation calculations.">>
		MinRadius;

		Field<&PointLightComponent::Radius, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The maximum radius for light attenuation calculations.">>
		Radius;

		Field<&PointLightComponent::Falloff, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The falloff exponent for light attenuation.">>
		Falloff;

		Field<&PointLightComponent::SourceSize, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The size of the light source affecting soft shadow calculations.">>
		SourceSize;

		Field<&PointLightComponent::SpecularScale, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"Multiplier for specular highlights produced by the light. Any value other than 1.0 breaks energy conservation.">>
		SpecularScale;

		Field<&PointLightComponent::CastShadows, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light casts shadows.">>
		CastShadows;
	};

	template<>
	struct Reflector<SpotLightComponent>
	{
		using Type = Type<SpotLightComponent, Scriptable, Icon<EditorIcons::Lightbulb>>;

		Field<&SpotLightComponent::Enabled, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light is enabled.">>
		Enabled;

		Field<&SpotLightComponent::Color, Serializable, Editable, Scriptable,
		Tooltip<"The color of the light.">>
		Color;

		Field<&SpotLightComponent::Intensity, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The intensity of the light. Color * Intensity defines the final color of the light.">>
		Intensity;

		Field<&SpotLightComponent::Range, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The range of the spotlight.">>
		Range;

		Field<&SpotLightComponent::InnerConeAngleDeg, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The inner cone angle (in degrees) defining the spotlight's bright center.">>
		InnerConeAngleDeg;

		Field<&SpotLightComponent::OuterConeAngleDeg, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The outer cone angle (in degrees) defining the spotlight's outer boundary.">>
		OuterConeAngleDeg;

		Field<&SpotLightComponent::Falloff, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The falloff exponent for light attenuation.">>
		Falloff;

		Field<&SpotLightComponent::SourceSize, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The size of the light source affecting soft shadow calculations.">>
		SourceSize;

		Field<&SpotLightComponent::SpecularScale, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"Multiplier for specular highlights produced by the light. Any value other than 1.0 breaks energy conservation.">>
		SpecularScale;

		Field<&SpotLightComponent::CastShadows, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light casts shadows.">>
		CastShadows;
	};

	template<>
	struct Reflector<SkyboxComponent>
	{
		using Type = Type<SkyboxComponent, Scriptable, Icon<EditorIcons::CloudSun>>;

		Field<&SkyboxComponent::EnvironmentMap, Serializable, Editable, Scriptable,
		Tooltip<"The environment map asset used for the skybox.">>
		EnvironmentMap;

		Field<&SkyboxComponent::Exposure, Serializable, Editable, Scriptable,
		Tooltip<"The exposure level of the skybox.">>
		Exposure;

		Field<&SkyboxComponent::Gamma, Serializable, Editable, Scriptable,
		Tooltip<"The gamma correction applied to the skybox.">>
		Gamma;

		Field<&SkyboxComponent::Blur, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The amount of blur applied to the skybox.">>
		Blur;

		Field<&SkyboxComponent::Intensity, Serializable, Editable, Scriptable, Min<0.0f>,
		Tooltip<"The intensity multiplier for the skybox.">>
		Intensity;
	};

} // namespace Tridium::Meta