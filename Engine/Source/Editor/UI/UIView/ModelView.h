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
	/* ModelView ���
	1. ������ ���콺 �������� Model Ȯ�� ���
	2. Material �ٲ㳢�鼭 Ȯ�� ���( Material ���� �˻� ��� �߰�)
	3. Mesh ���� ��� ( Vertex ��, ���� ��� )
	4. FBX import �� skeletal, animation �� ���� �� �ֵ��� ��� �߰�
	*/
	/* MaterialView ���
	1. Material Script ������� ( �����ɸ��°Ͱ����� �н� ) 
	2. Material ������, Mesh �׽�Ʈ ����, ��� ��� �߰�
	*/
	/* PrefabView ���
	
	*/

	class ModelView : public UIView
	{
		JGCLASS
	private:

		const JVector2 mResolution = JVector2(300, 300);
		const JVector2 mImageSize  = mResolution * 1.5f;
		JVector2 mMousePosition;
		JVector2 mPrevMousePosition;
		JVector2 mModelRotation;



		SharedPtr<ITexture>		 mSceneTexture;
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
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override { }
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override { }
	private:
		void Top_OnGUI();
		void Mid_OnGUI();
		void Bottom_OnGUI();

		void MidLeft_OnGUI();
		void MidRight_OnGUI();
	private:
		void PushSceneObject();
		void Rendering();
		void UpdateScene();

		
	public:
		void SetModel(const String& path);
		void SetMaterial(i32 index, const String& path);
	};
}