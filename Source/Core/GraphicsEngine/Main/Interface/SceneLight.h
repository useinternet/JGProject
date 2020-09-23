#pragma once
#include "GEObject.h"
#include "Texture.h"


namespace GE
{
	enum class GRAPHICSENGINE_API LightType
	{
		Point,
		Spot,
		Directional
	};
	enum class GRAPHICSENGINE_API LightMovementType
	{
		Static,
		Dyanmic
	};
	class GRAPHICSENGINE_API SceneLight : public GEObject
	{
		LightMovementType m_LightMovementType = LightMovementType::Static;

	public:
		LightMovementType GetMovementType() const {
			return m_LightMovementType;
		}
		void SetMovementType(LightMovementType type) {
			m_LightMovementType = type;
		}
		virtual LightType GetType() const = 0;
		virtual ~SceneLight() {}

	};


	class GRAPHICSENGINE_API PointLight : public SceneLight
	{
	public:
		const SDPointLight& GetSDData() const {
			return m_SDLightData;
		}
		SDPointLight& GetSDDataRef() {
			return m_SDLightData;
		}
	public:
		virtual GE::Texture* GetShadowCubeMap() const = 0;
		virtual LightType GetType() const override {
			return LightType::Point;
		}
		virtual ~PointLight() {}
	private:
		SDPointLight m_SDLightData;
	};
	class GRAPHICSENGINE_API SpotLight : public SceneLight
	{
	public:
		const SDSpotLight& GetSDData() const {
			return m_SDLightData;
		}
		SDSpotLight& GetSDDataRef() {
			return m_SDLightData;
		}
	public:
		virtual GE::Texture* GetShadowMap() const = 0;
		virtual LightType GetType() const override {
			return LightType::Spot;
		}
		virtual ~SpotLight() {}
	private:
		SDSpotLight m_SDLightData;

	};
	class GRAPHICSENGINE_API DirectionalLight : public SceneLight
	{
	public:
		const SDDirectionalLight& GetSDData() const {
			return m_SDLightData;
		}
		SDDirectionalLight& GetSDDataRef() {
			return m_SDLightData;
		}
	public:
		virtual GE::Texture* GetShadowMap() const = 0;
		virtual LightType GetType() const override{
			return LightType::Directional;
		}
		virtual ~DirectionalLight() {}
	private:
		SDDirectionalLight m_SDLightData;
	};
}