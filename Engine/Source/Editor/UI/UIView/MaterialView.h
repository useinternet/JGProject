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
		class StaticRenderObject;
	}
	class JGScriptEditor;
	class MaterialView : public UIView
	{
		JGCLASS
	private:
		const JVector2 mResolution = JVector2(450, 450);
		const JVector2 mImageSize = mResolution;
		const f32 mFarZ = 1000000.0f;
		const f32 mNearZ = 1.0f;
		JVector3 mEyePos = JVector3(0, 0, -200.0f);

		JVector2 mMousePosition;
		JVector2 mPrevMousePosition;
		JVector2 mModelRotation;



		SharedPtr<ITexture>		    mSceneTexture;
		SharedPtr<Asset<IMesh>>     mMeshAsset = nullptr;
		SharedPtr<Asset<IMaterial>> mMaterialAsset = nullptr;
		UniquePtr<JGScriptEditor>  mScriptEditor;

		SharedPtr<Graphics::StaticRenderObject> mSkyBox;
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
	private:
		// 모델링 뷰
		void LeftTopOnGUI();
		// 프로퍼티, 에셋 수정
		void LeftBottomOnGUI();
		// 스크립트
		void RightTopOnGUI();
		// 컴파일 및 에러 오류로그
		void RightBottomOnGUI();
	private:
		void PushSceneObject();
		void Rendering();
		void UpdateScene();
	public:
		void SetMaterial(const String& path);
		void SetMesh(const String& path);
	};

}