#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	/* MaterialView 기능
1. Material Script 편집기능 ( 오래걸리는것같으면 패스 )
2. Material 컴파일, Mesh 테스트 적용, 뷰어 기능 추가
*/
	namespace Graphics
	{
		class Scene;
	}
	class MaterialView : public UIView
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
		SharedPtr<Asset<IMaterial>> mMaterialAsset = nullptr;



		Graphics::Scene* mScene = nullptr;
	public:
		MaterialView();
		virtual ~MaterialView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void UpdateScene();
		void SetMaterial(const String& path);
		void SetMesh(const String& path);
	};

}