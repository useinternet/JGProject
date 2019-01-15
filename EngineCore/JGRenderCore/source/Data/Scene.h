#pragma once
#include"DxCommon/DxCommon.h"
#include"JGRCObject.h"
#include"JGMaterial.h"
#include"JGMesh.h"
#include"LightManager.h"
#include"DxCore/DxSetting.h"
/*
라이트 정리
노멀 적용 물 적용 예제 변경
그리고 오브젝트 Static, Dynamic 분리


--- 해야할거 ( 언제 걸릴지 모른다. ) 
6. 입방체 맵 특정 상수에의 의하여 블러 처리( 가장자리 블러 아니면 가우스 블러 ) <- 그리고 평면 입방체도 
적용할수있게끔 수정 ( unitrayDir 에 현재 posW 나 posH 에서 center으로 향하는 벡터 를 너보자 )
7. 그림자 블러처리 ( 또한 그림자 깊이 편차 값 실시간 으로 수정할수있게 변경)
8. 인스턴스 오브젝트 구현
9. 오브젝트 동적 생성 할수있게 리팩토링( 또한 오브젝트 타입별 나누고 PSO 별 나누분류 작업 리펙토링)
10. 옥트리를 이용한 프리스텀 컬링 및 피킹 시스템 구현
11. 그 후 로그 및 프로파일링 및 FBX 불러오기 (JGMesh 개편) 
12. 코드 다듬고 애니메이션( 평평 물체 반사)
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
		std::shared_ptr<class Shader> m_SceneShader;
		ID3D12PipelineState* m_ScenePSO = nullptr;
		//  SSAO
		std::shared_ptr<class SSAO> m_SSAO;
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
		std::vector<std::unique_ptr<JGMesh>>         m_MeshMems;
		std::unordered_map<std::string, JGMesh*>     m_Meshs;

		// 텍스쳐 경로
		std::vector<TexturePack> m_TextureDatas;

		// 스카이 박스 및 씬 전체르 감싸는 구
		DirectX::BoundingSphere m_SceneSphere;
		std::wstring m_SkyShaderPath = L"../Contents/Engine/Shaders/Sky.hlsl";
		std::unordered_map<std::wstring, JGRCObject*> m_SkyBox;
		JGRCObject* m_MainSkyBox = nullptr;

		// 라이트 
		std::unique_ptr<LightManager> m_LightManager;
		std::unique_ptr<JGRCObject>   m_DebugObj;
	private:
		Scene(const Scene& rhs) = delete;
		Scene& operator=(const Scene& rhs) = delete;
	public:
		Scene(class DxCore* core);
		void BuildScene();
		void OnReSize(UINT width, UINT height);
		void Update(const GameTimer& gt);
		void Draw();
	public:
		JGRCObject* CreateObject(EObjType Type = EObjType::Static);
		JGRCObject* CreateSkyBox(const std::wstring& texturepath);
		JGMaterial* AddMaterial(const MaterialDesc& Desc);
		JGMesh*     AddMesh();
		Camera*     AddCamera();
		PassData*   AddPassData();
		JGLight*    AddLight(ELightType type, ELightExercise extype = ELightExercise::Static);
		void        AddTexture(const std::wstring& TexturePath, ETextureType type = ETextureType::Default);
	
	public:
		Camera*   GetMainCamera() const      { return m_MainCamera; }
		void      SetMainCamera(Camera* cam) { m_MainCamera = cam; }
		JGRCObject* GetMainSkyBox() const    { return m_MainSkyBox; }
		void      SetMainSkyBox(JGRCObject* skybox)  { 
			if (skybox->GetMaterial()->GetDesc()->Mode == EPSOMode::SKYBOX)
				m_MainSkyBox = skybox;	}
		const DxSetting& GetSetting() const         { return m_SceneConfig; }
		ID3D12PipelineState* GetScenePSO() const   { return m_ScenePSO; }
	public:
		const cbPassConstant& MainPassData()  const    { return m_MainPass->Data; }
		PassData*             GetMainPass()            { return m_MainPass; }
		D3D12_GPU_VIRTUAL_ADDRESS MainPassHandle();
		const DirectX::BoundingSphere& GetSceneSphere()      const { return m_SceneSphere; }
		std::vector<std::unique_ptr<JGRCObject>>& GetArray()       { return m_ObjectMems; }
		std::vector<JGRCObject*>& GetArray(EObjType objType, EPSOMode mode)  
		{ return m_ObjectArray[objType][mode]; }
	private:
		void CreateDebugObj();
		void MainPassUpdate(const GameTimer& gt);
		void InitStaticMemberVar();
	};
}