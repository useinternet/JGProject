#pragma once

#include "GEObject.h"
#include "SceneCamera.h"
/*
Class : Scene

ex : 카메라 설정, 컬링 설정, 렌더러 설정, 씬 오브젝트 배치



*/
class GraphicsIF;
namespace GE
{
	class SceneCamera;
	class SceneObject;
	class SceneLight;
	class DirectionalLight;
	class PointLight;
	class SpotLight;
	class Texture;
	class GRAPHICSENGINE_API Scene : public GEObject
	{
		friend GraphicsIF;
	public:
		virtual void BindCamera(SceneCamera* cam) = 0;
		virtual void UnBindCamera() = 0;

		virtual void PushObject(SceneObject* sceneObject) = 0;
		virtual void PushObjects(const std::vector<SceneObject*>& sceneObject) = 0;
		virtual void PushLight(SceneLight* light) = 0;



		virtual void FlushObjects() = 0;

		virtual SceneCamera* GetBindedCamera() const = 0;
		virtual const std::vector<SceneObject*>& GetPushedObjects() const = 0;
		virtual DirectionalLight* GetPushedDirectionalLight() const = 0;
		virtual const std::vector<GE::PointLight*>& GetPushedPointLights() const = 0;
		virtual const std::vector<GE::SpotLight*>&  GetPushedSpotLights() const = 0;

		virtual Texture* GetSceneTexture() const = 0;
		virtual ~Scene() {}

		RendererMode GetRendererMode() const {
			return m_RendererMode;
		}
	private:
		RendererMode m_RendererMode;
	};



	
}
