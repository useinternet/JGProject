#pragma once
#include"DxCommon/DxCommon.h"

#include"Animation/JGAnimation.h"
#include"DxCore/SceneConfig.h"

namespace JGRC
{
	class DebugBox;
	class SceneData;
	class CommandListManager;
	class ScreenManager;
	class GpuCpuSynchronizer;
	class DxDevice;
	class SceneObject;
	class JGMaterial;
	class JGStaticMesh;
	class JGSkeletalMesh;
	class LightManager;
	class JGLight;

	enum class ELightType;
	enum class ELightExercise;
	enum class EObjectType;
	enum class EObjectRenderMode;
	class RCORE_EXPORT Scene
	{
		typedef std::unordered_map<EObjectType,
			    std::unordered_map<EPSOMode,
			    std::vector<SceneObject*>>>
			    ObjectArray;
	private:
		std::string m_Name;
		DxDevice* m_DxDevice = nullptr;
		SceneConfig m_SceneConfig;

		// 메모리 및 프레임 관리자
		ResourceManager*            m_ResourceManager;
		CommandListManager*         m_CmdListManager;
		ScreenManager*              m_ScreenManager;
		std::shared_ptr<SceneData>    m_SceneData;
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
		std::vector<SceneObject*>      m_ObjectMems;
		ObjectArray m_ObjectArray;
		std::vector<std::shared_ptr<DebugBox>> m_DebugMems;
		// 머터리얼
		std::vector<JGMaterial*>      m_MaterialMems;
		// 애니메이션
		std::vector<std::unique_ptr<JGAnimation>>     m_AnimationMems;
		std::unordered_map<std::string, JGAnimation*> m_Animations;
		// 텍스쳐 경로
		std::vector<TexturePack> m_TextureDatas;
		// 스카이 박스 및 씬 전체르 감싸는 구
		DirectX::BoundingSphere m_SceneSphere;
		std::wstring m_SkyShaderPath = L"../Contents/Engine/Shaders/Sky.hlsl";
		std::unordered_map<std::wstring, SceneObject*> m_SkyBox;
		SceneObject* m_MainSkyBox = nullptr;
		// 라이트 
		std::shared_ptr<LightManager> m_LightManager;
		//
	private:
		Scene(const Scene& rhs) = delete;
		Scene& operator=(const Scene& rhs) = delete;
	public:
		Scene(const std::string& name, const SceneConfig& config);
		void BuildScene();
		void OnReSize(UINT width, UINT height);
		void Update(const GameTimer& gt);
		void Draw();
		void SceneObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjectRenderMode Mode, bool bDebug = false);
		void SceneStaticObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjectRenderMode Mode);
		void SceneDynamicObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjectRenderMode Mode);
	public:
		SceneObject*     CreateObject(const std::string& name, JGMaterial* mat, const std::string& matDataName, JGBaseMesh* mesh, const std::string& meshname, EObjectType Type);
		SceneObject*     CreateSkyBox(const std::wstring& texturepath);
		void            AddDebugBox(SceneObject* obj, const DirectX::XMFLOAT3& color, float thickness = 0);
		JGMaterial*     AddMaterial(const MaterialDesc& Desc);
		JGStaticMesh*   AddStaticMesh(const std::string& name);
		JGSkeletalMesh* AddSkeletalMesh(const std::string& name);
		std::string     AddAnimation(const std::string& path);
		Camera*         AddCamera();
		PassData*       AddPassData();
		JGLight*        AddLight(ELightType type, ELightExercise extype);
		void            AddTexture(const std::wstring& TexturePath, ETextureType type = ETextureType::Default);
	public:
		Camera*      GetMainCamera() const               { return m_MainCamera; }
		void         SetMainCamera(Camera* cam)          { m_MainCamera = cam; }
		SceneObject*  GetMainSkyBox() const;
		void         SetMainSkyBox(SceneObject* skybox);
		const SceneConfig&   GetConfig() const           { return m_SceneConfig; }
		ID3D12PipelineState* GetScenePSO() const         { return m_ScenePSO.Get(); }
		JGAnimation*  GetAnimation(const std::string& name);
		SceneData*    GetSceneData() const;
		LightManager* GetLightManager();
		const std::string& GetName() const { return m_Name; }
	public:
		const PassConstantData& MainPassData()  const    { return m_MainPass->Get(); }
		PassData*             GetMainPass()            { return m_MainPass; }
		D3D12_GPU_VIRTUAL_ADDRESS MainPassHandle();
		const DirectX::BoundingSphere& GetSceneSphere()      const { return m_SceneSphere; }
	private:
		void MainPassUpdate(const GameTimer& gt);
	};
}