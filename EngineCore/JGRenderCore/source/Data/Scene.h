#pragma once
#include"DxCommon/DxCommon.h"
#include"LightManager.h"
#include"JGMaterial.h"
#include"Animation/JGAnimation.h"
#include"JGRCObject.h"
#include"Mesh/JGStaticMesh.h"
#include"Mesh/JGSkeletalMesh.h"
#include"Debug/DebugBox.h"
#include"Debug/DebugScreen.h"
#include"DxCore/DxSetting.h"
/*
--- 해야할거 ( 언제 걸릴지 모른다. ) 
1. Tangent 계산
2. Object 당 메시하나 머터리얼 하나
3. anim, mesh, texture, material fbx에서 추출
4. 인터페이스 클래스
5. JGRenderCore밖에서 샘플 생성
*/
namespace JGRC
{
	class SceneData;
	class CommandListManager;
	class ScreenManager;
	class GpuCpuSynchronizer;
	class DxCore;
	class RCORE_EXPORT Scene
	{
		typedef std::unordered_map<EObjType,
			    std::unordered_map<EPSOMode,
			    std::vector<JGRCObject*>>>
			    ObjectArray;
	private:
		std::string m_Name;
		DxCore* m_DxCore = nullptr;
		DxSetting m_SceneConfig;

		// 메모리 및 프레임 관리자
		ResourceManager*            m_ResourceManager;
		CommandListManager*         m_CmdListManager;
		ScreenManager*              m_ScreenManager;
		GpuCpuSynchronizer*         m_GCS;
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
		PassData* m_MainPass = nullptr;

		// 카메라
		std::vector<std::unique_ptr<Camera>> m_Cameras;
		Camera* m_MainCamera = nullptr;


		// 오브젝트
		std::vector<JGRCObject*>      m_ObjectMems;
		ObjectArray m_ObjectArray;
		std::vector<std::unique_ptr<DebugBox>> m_DebugMems;

		// 머터리얼
		std::vector<JGMaterial*>           m_MaterialMems;

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
		Scene(const std::string& name, DxCore* core, ResourceManager* manager,
			CommandListManager* cmdManager, ScreenManager* screenManager,
			GpuCpuSynchronizer* gcs);
		void BuildScene();
		void OnReSize(UINT width, UINT height);
		void Update(const GameTimer& gt);
		void Draw();
		void SceneObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode, bool bDebug = false);
		void SceneStaticObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode);
		void SceneDynamicObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode);
	public:
		JGRCObject*     CreateObject(JGMaterial* mat, JGBaseMesh* mesh, const std::string& meshname, EObjType Type = EObjType::Static);
		JGRCObject*     CreateSkyBox(const std::wstring& texturepath);
		void            AddDebugBox(JGRCObject* obj, const DirectX::XMFLOAT3& color, float thickness = 0);
		JGMaterial*     AddMaterial(const MaterialDesc& Desc);
		JGStaticMesh*   AddStaticMesh();
		JGSkeletalMesh* AddSkeletalMesh();
		std::string AddAnimation(const std::string& path);
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
		SceneData* GetSceneData() const;
		const std::string& GetName() const { return m_Name; }
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