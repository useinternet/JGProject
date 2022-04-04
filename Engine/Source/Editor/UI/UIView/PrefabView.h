#pragma once
#include "UI/UIView.h"


namespace JG
{
	namespace Graphics
	{
		class Scene;
	}

	class PrefabView : public UIView
	{
		JGCLASS
	private:
		const JVector2 mResolution = JVector2(300, 300);
		const JVector2 mImageSize = mResolution * 1.5f;
		JVector2 mMousePosition;
		JVector2 mPrevMousePosition;
		JVector2 mModelRotation;



		SharedPtr<ITexture>		 mSceneTexture;
		SharedPtr<Asset<IMesh>>  mMeshAsset = nullptr;
		List<SharedPtr<Asset<IMaterial>>> mMaterialAssetList;



		Graphics::Scene* mScene = nullptr;
	public:
		PrefabView();
		virtual ~PrefabView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void SetMaterial(const String& path);
		void SetMesh(const String& path);
	};

}