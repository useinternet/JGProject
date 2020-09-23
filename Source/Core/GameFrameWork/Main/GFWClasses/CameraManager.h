#pragma once
#include "GFWObject.h"


namespace GFW
{

	class GAMEFRAMEWORK_API CameraManager : public GFWObject
	{
		friend class GFWIF;
		/*
		플레이어 카메라 -> 컨트롤러에 조종되는 카메라,
		월드     카메라 -> 월드 카메라
		
		*/
	public:
		class Camera
		{
		public:
			GE::Scene*      scene = nullptr;
			GE::SceneCamera camera;


			bool Enable = true;
		};
	public:
		CameraManager() : GFWObject(TT("CameraManager")) {}
	protected:
		virtual void Awake()   override;
		virtual void Start()   override {}
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		Camera* GetWorldCamera() const {
			return m_WorldCamera.get();
		}
	private:
		std::unique_ptr<Camera> m_WorldCamera;
	};
}