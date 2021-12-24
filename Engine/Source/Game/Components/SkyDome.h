#pragma once
#include "GameComponent.h"


namespace JG
{
	class ITexture;
	class IMesh;




	class SkyDome : public GameComponent
	{
		JGCLASS

		SharedPtr<IMesh> mMesh = nullptr;
		SharedPtr<IMaterial> mMaterial = nullptr;
		Asset<ITexture>* mCubeMap = nullptr;
		SharedPtr<ScheduleHandle> mPushRenderSceneObjectScheduleHandle;
	public:
		virtual ~SkyDome();
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	private:
		EScheduleResult SkyDome::PushRenderSceneObject();
		void CreateGeometry();
	};
}