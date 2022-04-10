#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	/* MaterialView ���
1. Material Script ������� ( �����ɸ��°Ͱ����� �н� )
2. Material ������, Mesh �׽�Ʈ ����, ��� ��� �߰�
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
		// �𵨸� ��
		void LeftTopOnGUI();
		// ������Ƽ, ���� ����
		void LeftBottomOnGUI();
		// ��ũ��Ʈ
		void RightTopOnGUI();
		// ������ �� ���� �����α�
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