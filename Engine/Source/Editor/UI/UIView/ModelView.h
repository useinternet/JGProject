#pragma once
#include "JGCore.h"
#include "UI/UIView.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	namespace Graphics
	{
		class Scene;
		class StaticRenderObject;

	}
	class ModelView : public UIView
	{
		JGCLASS
	private:

		const JVector2 mResolution = JVector2(450, 450);
		const JVector2 mImageSize = mResolution;
		const f32 mFarZ  = 1000000.0f;
		const f32 mNearZ = 1.0f;
		JVector3 mEyePos = JVector3(0, 0, -200.0f);

		JVector2 mMousePosition;
		JVector2 mPrevMousePosition;
		JVector2 mModelRotation;



		SharedPtr<ITexture>		 mSceneTexture;
		SharedPtr<Asset<IMesh>>  mMeshAsset  = nullptr;
		List<SharedPtr<Asset<IMaterial>>> mMaterialAssetList;
		
		
		SharedPtr<Graphics::StaticRenderObject> mSkyBox;
		Graphics::Scene* mScene = nullptr;
	public:
		ModelView();
		virtual ~ModelView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void PreOnGUI() override;
		virtual void LateOnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	private:
		void Left_OnGUI();
		void Right_OnGUI();
	private:
		void PushSceneObject();
		void Rendering();
		void UpdateScene();
	public:
		void SetModel(const String& path);
		void SetMaterial(i32 index, const String& path);
	};
}