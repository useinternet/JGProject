#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	namespace Graphics
	{
		class Scene;
		class StaticRenderObject;
	}
	class TextEditor;
	class MaterialView : public UIView
	{
		JGCLASS
	private:
		const JVector2 mResolution = JVector2(450, 450);
		const JVector2 mImageSize  = mResolution;
		const f32 mRightBottomHeight = 230.0f;
		f32 mWindowHeight			 = 0.0f;



		const f32 mFarZ = 1000000.0f;
		const f32 mNearZ = 1.0f;
		JVector3 mEyePos = JVector3(0, 0, -200.0f);

		JVector2 mMousePosition;
		JVector2 mPrevMousePosition;
		JVector2 mModelRotation;



		SharedPtr<ITexture>		    mSceneTexture;
		SharedPtr<Asset<IMesh>>     mMeshAsset = nullptr;
		SharedPtr<Asset<IMaterial>> mMaterialAsset = nullptr;
		UniquePtr<TextEditor>		mTextEditor;

		List<String> mMaterialLogs;

		SharedPtr<Graphics::StaticRenderObject> mSkyBox;
		Graphics::Scene* mScene = nullptr;

		StorableString mFailedCompileShaderCode;
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
		void LoadTextEditor(const String& text);
		void PushSceneObject();
		void Rendering();
		void UpdateScene();

		void SaveAndApplyScript();
		bool CompileShader();

		void AddLog(const String& log);
		void ClearLogs();
	public:
		void SetMaterial(const String& path);
		void SetMesh(const String& path);
	};

}