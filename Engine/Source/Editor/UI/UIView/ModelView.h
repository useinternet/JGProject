#pragma once
#include "JGCore.h"
#include "UI/UIView.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	namespace Graphics
	{
		class Scene;

	}
	class ModelView : public UIView
	{
		JGCLASS
	private:

		const JVector2 mResolution = JVector2(300, 300);

		SharedPtr<ITexture> mSceneTexture;
		SharedPtr<Asset<IMesh>>  mMeshAsset  = nullptr;
		List<SharedPtr<Asset<IMaterial>>> mMaterialAssetList;
		
		
	
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
	private:
		void Top_OnGUI();
		void Scene_OnGUI();
		void Right_OnGUI();
		void Bottom_OnGUI();
	private:
		void PushSceneObject();
		void Rendering();
	public:
		void SetModel(const String& path);
	};
}