#pragma once
#include <Tridium/Scene/Component.h>
#include <Tridium/Asset/Asset.h>
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Graphics/Color.h>

namespace Tridium {

	//=================================================================================================
	// Camera Component: Defines a camera in the scene.
	//=================================================================================================
	struct CameraComponent
	{
		enum class EProjectionType { Perspective = 0, Orthographic = 1 };

		EProjectionType ProjectionType = EProjectionType::Perspective;
		Vector2 ViewportSize{};

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

		float AspectRatio() const { return ViewportSize.X / ViewportSize.Y; }
		Matrix4 CalculatePerspective() const;
		Matrix4 CalculateOrthographic() const;
		Matrix4 CalculateProjection() const { return ProjectionType == EProjectionType::Perspective ? CalculatePerspective() : CalculateOrthographic(); }
	};

	//=================================================================================================
	// Static Mesh Component: Renders a static mesh in the scene.
	//=================================================================================================
	struct StaticMeshComponent
	{
		AssetHandle<StaticMesh> Mesh;
		bool CastShadows = true;
	};

	//=================================================================================================
	// Directional Light Component: A light that has a direction but no specific position.
	//=================================================================================================
	struct DirectionalLightComponent
	{
		// The color of the light, represented as RGB values.
		Color3 Color{ 1.0f, 1.0f, 1.0f };

		// The intensity of the light, affecting its brightness.
		float Intensity{ 1.0f };

		// A scale factor for specular highlights produced by the light.
		// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
		float SpecularScale{ 1.0f };

		// Whether the light casts shadows in the scene.
		bool CastShadows = true;
	};

	//=================================================================================================
	// Point Light Component: 
	// A light that emits light uniformly in all directions from a single point in space.
	//=================================================================================================
	struct PointLightComponent
	{
		// The color of the light, represented as RGB values.
		Color3 Color{ 1.0f, 1.0f, 1.0f };

		// The intensity of the light, affecting its brightness.
		float Intensity{ 1.0f };

		// The minimum radius for light attenuation calculations.
		float MinRadius{ 0.001f };

		// The maximum radius for light attenuation calculations.
		float Radius{ 25.0f };
		// The falloff exponent for light attenuation, controlling how quickly the light diminishes with distance.
		float Falloff{ 1.f };

		// The size of the light source, affecting soft shadow calculations.
		float SourceSize{ 0.1f };

		// A scale factor for specular highlights produced by the light.
		// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
		float SpecularScale{ 1.0f };
		
		// Whether the light casts shadows in the scene.
		bool CastShadows = true;
	};

	//=================================================================================================
	// Spot Light Component: A light that emits light in a specific direction within a cone,
	// simulating a flashlight or spotlight.
	//=================================================================================================
	struct SpotLightComponent
	{
		// The color of the light, represented as RGB values.
		Color3 Color{ 1.0f, 1.0f, 1.0f };

		// The intensity of the light, affecting its brightness.
		float Intensity{ 1.0f };

		// The minimum radius for light attenuation calculations.
		float Range{ 0.1f };

		// The attenuation factor based on the angle between the light's direction and the point being lit.
		float AngleAttenuation{ 0.0f };

		// The angle (in radians) defining the cone of the spotlight.
		float Angle{ 0.0f };

		// The falloff exponent for light attenuation, controlling how quickly the light diminishes with distance.
		float Falloff{ 1.0f };

		// A scale factor for specular highlights produced by the light.
		// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
		float SpecularScale{ 1.0f };

		// Whether the light casts shadows in the scene.
		bool CastShadows = true;
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

		Field<&CameraComponent::ViewportSize, Serializable, Editable, Scriptable,
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

		//Field<CameraComponent::EProjectionType::Perspective, Scriptable>
		//Perspective;
		//
		//Field<CameraComponent::EProjectionType::Orthographic, Scriptable>
		//Orthographic;
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

		Field<&DirectionalLightComponent::Color, Serializable, Editable, Scriptable,
		Tooltip<"The color of the light.">>
		Color;

		Field<&DirectionalLightComponent::Intensity, Serializable, Editable, Scriptable,
		Tooltip<"The intensity of the light. Color * Intensity defines the final color of the light.">>
		Intensity;

		Field<&DirectionalLightComponent::SpecularScale, Serializable, Editable, Scriptable,
		Tooltip<"Multiplier for specular highlights produced by the light. Any value other than 1.0 breaks energy conservation.">>
		SpecularScale;

		Field<&DirectionalLightComponent::CastShadows, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light casts shadows.">>
		CastShadows;
	};

	template<>
	struct Reflector<PointLightComponent>
	{
		using Type = Type<PointLightComponent, Scriptable, Icon<EditorIcons::Lightbulb>>;

		Field<&PointLightComponent::Color, Serializable, Editable, Scriptable,
		Tooltip<"The color of the light.">>
		Color;

		Field<&PointLightComponent::Intensity, Serializable, Editable, Scriptable,
		Tooltip<"The intensity of the light. Color * Intensity defines the final color of the light.">>
		Intensity;

		Field<&PointLightComponent::MinRadius, Serializable, Editable, Scriptable,
		Tooltip<"The minimum radius for light attenuation calculations.">>
		MinRadius;

		Field<&PointLightComponent::Radius, Serializable, Editable, Scriptable,
		Tooltip<"The maximum radius for light attenuation calculations.">>
		Radius;

		Field<&PointLightComponent::Falloff, Serializable, Editable, Scriptable,
		Tooltip<"The falloff exponent for light attenuation.">>
		Falloff;

		Field<&PointLightComponent::SourceSize, Serializable, Editable, Scriptable,
		Tooltip<"The size of the light source affecting soft shadow calculations.">>
		SourceSize;

		Field<&PointLightComponent::SpecularScale, Serializable, Editable, Scriptable,
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

		Field<&SpotLightComponent::Color, Serializable, Editable, Scriptable,
		Tooltip<"The color of the light.">>
		Color;

		Field<&SpotLightComponent::Intensity, Serializable, Editable, Scriptable,
		Tooltip<"The intensity of the light. Color * Intensity defines the final color of the light.">>
		Intensity;

		Field<&SpotLightComponent::Range, Serializable, Editable, Scriptable,
		Tooltip<"The range of the spotlight.">>
		Range;

		Field<&SpotLightComponent::AngleAttenuation, Serializable, Editable, Scriptable,
		Tooltip<"The attenuation factor based on the angle between the light's direction and the point being lit.">>
		AngleAttenuation;

		Field<&SpotLightComponent::Angle, Serializable, Editable, Scriptable,
		Tooltip<"The angle (in radians) defining the cone of the spotlight.">>
		Angle;

		Field<&SpotLightComponent::Falloff, Serializable, Editable, Scriptable,
		Tooltip<"The falloff exponent for light attenuation.">>
		Falloff;

		Field<&SpotLightComponent::SpecularScale, Serializable, Editable, Scriptable,
		Tooltip<"Multiplier for specular highlights produced by the light. Any value other than 1.0 breaks energy conservation.">>
		SpecularScale;

		Field<&SpotLightComponent::CastShadows, Serializable, Editable, Scriptable,
		Tooltip<"Whether the light casts shadows.">>
		CastShadows;
	};

} // namespace Tridium::Meta