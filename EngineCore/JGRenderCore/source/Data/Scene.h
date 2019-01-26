#pragma once
#include"DxCommon/DxCommon.h"
#include"LightManager.h"
#include"UserInterface/JGRCInterfaceClass.h"
#include"JGMaterial.h"
#include"Animation/JGAnimation.h"
#include"JGRCObject.h"
#include"Mesh/JGStaticMesh.h"
#include"Mesh/JGSkeletalMesh.h"
#include"DxCore/DxSetting.h"
/*
--- 해야할거 ( 언제 걸릴지 모른다. ) 
1. Tangent 계산
2. Object 당 메시하나 머터리얼 하나
3. 인터페이스 클래스
4. JGRenderCore밖에서 샘플 생성
5. anim, mesh, texture fbx에서 추출
*/
namespace JGRC
{
	class RCORE_EXPORT Scene
	{
		typedef std::unordered_map<EObjType,
			    std::unordered_map<EPSOMode,
			    std::vector<JGRCObject*>>>
			    ObjectArray;
	private:
		class DxCore* m_DxCore = nullptr;
		DxSetting m_SceneConfig;

		// 메모리 및 프레임 관리자
		std::unique_ptr<ResourceManager>            m_ResourceManager;
		std::unique_ptr<EngineFrameResourceManager> m_FrameResourceManager;
		std::shared_ptr<class SceneData>            m_SceneData;
		std::shared_ptr<class CommonShaderRootSignature> m_CommonShaderRootSig;
		std::shared_ptr<class CommonSkinnedShaderRootSignature> m_CommonSkinnedShadeRootSig;
		std::shared_ptr<class Shader> m_SceneShader;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_ScenePSO = nullptr;
		//  SSAO
		std::shared_ptr<class SSAO> m_SSAO;
		std::shared_ptr<class BlurFilter> m_Blur;

		// 메인 상수 패스 
		std::vector<std::unique_ptr<PassData>> m_PassDatas;
		UINT      m_PassCBIndex = -1;
		PassData* m_MainPass = nullptr;

		// 카메라
		std::vector<std::unique_ptr<Camera>> m_Cameras;
		Camera* m_MainCamera = nullptr;

		// 오브젝트
		std::vector<std::unique_ptr<JGRCObject>>      m_ObjectMems;
		ObjectArray m_ObjectArray;
		UINT m_ObjIndex = -1;

		// 머터리얼
		std::unique_ptr<JGMaterialCreater>           m_MaterialCreater;
		std::unordered_map<std::string, JGMaterial*> m_Materials;

		// 메시
		std::vector<std::unique_ptr<JGBaseMesh>>         m_MeshMems;
		std::unordered_map<std::string, JGBaseMesh*>     m_Meshs;

		// 애니메이션
		std::vector<std::unique_ptr<JGAnimation>>     m_AnimationMems;
		std::unordered_map<std::string, JGAnimation*> m_Animations;

		// 텍스쳐 경로
		std::vector<TexturePack> m_TextureDatas;

		// 스카이 박스 및 씬 전체르 감싸는 구
		DirectX::BoundingSphere m_SceneSphere;
		std::wstring m_SkyShaderPath = L"../Contents/Engine/Shaders/Sky.hlsl";
		std::unordered_map<std::wstring, JGRCObject*> m_SkyBox;
		JGRCObject* m_MainSkyBox = nullptr;
		// 라이트 
		std::unique_ptr<LightManager> m_LightManager;
		//
	private:
		Scene(const Scene& rhs) = delete;
		Scene& operator=(const Scene& rhs) = delete;
	public:
		Scene(class DxCore* core);
		void BuildScene();
		void OnReSize(UINT width, UINT height);
		void Update(const GameTimer& gt);
		void Draw();
		void SceneObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode);
		void SceneStaticObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode);
		void SceneDynamicObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode);
	public:
		JGRCObject*     CreateObject(JGMaterial* mat, JGBaseMesh* mesh, EObjType Type = EObjType::Static);
		JGRCObject*     CreateSkyBox(const std::wstring& texturepath);
		JGMaterial*     AddMaterial(const MaterialDesc& Desc);
		JGStaticMesh*   AddStaticMesh();
		JGSkeletalMesh* AddSkeletalMesh();
		std::vector<std::string> AddAnimation(const std::string& path);
		Camera*     AddCamera();
		PassData*   AddPassData();
		JGLight*    AddLight(ELightType type, ELightExercise extype = ELightExercise::Static);
		void        AddTexture(const std::wstring& TexturePath, ETextureType type = ETextureType::Default);
		void        SettingDefaultSceneBuffer(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource);
	public:
		Camera*      GetMainCamera() const               { return m_MainCamera; }
		void         SetMainCamera(Camera* cam)          { m_MainCamera = cam; }
		JGRCObject*  GetMainSkyBox() const               { return m_MainSkyBox; }
		void         SetMainSkyBox(JGRCObject* skybox)   {  if (skybox->GetMaterial()->GetDesc()->Mode == EPSOMode::SKYBOX) 	m_MainSkyBox = skybox;	}
		const DxSetting& GetSetting() const              { return m_SceneConfig; }
		ID3D12PipelineState* GetScenePSO() const         { return m_ScenePSO.Get(); }
		JGAnimation* GetAnimation(const std::string& name);

	public:
		CommonShaderRootSignature* GetRootSig();
		CommonSkinnedShaderRootSignature* GetSkinnedRootSig();
	public:
		const cbPassConstant& MainPassData()  const    { return m_MainPass->Data; }
		PassData*             GetMainPass()            { return m_MainPass; }
		D3D12_GPU_VIRTUAL_ADDRESS MainPassHandle();
		const DirectX::BoundingSphere& GetSceneSphere()      const { return m_SceneSphere; }
		std::vector<JGRCObject*>& GetArray(EObjType objType, EPSOMode mode);
	private:
		void MainPassUpdate(const GameTimer& gt);
		void InitStaticMemberVar();
	};
}