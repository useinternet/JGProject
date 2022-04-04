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
	/* ModelView 기능
	1. 간단한 마우스 조작으로 Model 확인 기능
	2. Material 바꿔끼면서 확인 기능( Material 에셋 검색 기능 추가)
	3. Mesh 정보 출력 ( Vertex 수, 각종 등등 )
	4. FBX import 시 skeletal, animation 도 뽑을 수 있도록 기능 추가
	*/
	/* MaterialView 기능
	1. Material Script 편집기능 ( 오래걸리는것같으면 패스 ) 
	2. Material 컴파일, Mesh 테스트 적용, 뷰어 기능 추가
	*/
	/* PrefabView 기능
	
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