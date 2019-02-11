#include"Scene.h"
#include"DxCore/DxCore.h"
#include"SceneData.h"
#include"Shader/Shader.h"
#include"Shader/CommonShaderRootSignature.h"

#include"PostProcessing/ShadowMap.h"
#include"PostProcessing/BlurFilter.h"
#include"PostProcessing/SSAO.h"

#include"JGLight.h"
#include"CommonData.h"
#include"ResourceManagement/ResourceReader.h"
#include"Debug/DebugBox.h"
#include"DxCore/CommandListManager.h"
#include"DxCore/GpuCpuSynchronizer.h"
#include"DxCore/ScreenManager.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;

Scene::Scene(const string& name,
	DxCore* core, ResourceManager* manager, CommandListManager* cmdManager,
	ScreenManager* screenManager, GpuCpuSynchronizer* gcs)
{
	m_Name = name;
	m_DxCore = core;
	m_ResourceManager = manager;
	m_CmdListManager = cmdManager;
	m_ScreenManager = screenManager;
	m_GCS = gcs;


	m_SceneConfig = core->GetSettingDesc();
	// 각 씬 데이터 정적 멤버 초기화
	InitStaticMemberVar();
	// 프레임 리소스및 머터리얼 생성기 초기화
	m_FrameResourceManager = make_unique<EngineFrameResourceManager>();

	// 씬데이터 초기화 및 루트서명 / 씬 셰이더, pso 데이터 
	m_SceneData = make_unique<SceneData>();
	m_CommonShaderRootSig = make_unique<CommonShaderRootSignature>();
	m_CommonShaderRootSig->RootSign(m_DxCore->Device());
	m_CommonSkinnedShadeRootSig = make_unique<CommonSkinnedShaderRootSignature>();
	m_CommonSkinnedShadeRootSig->RootSign(m_DxCore->Device());



	Shader SceneShader(global_scene_hlsl_path, { EShaderType::Vertex, EShaderType::Pixel });

	m_ScenePSO = SceneShader.CompileAndConstrutPSO(EPSOMode::SCENE, m_CommonShaderRootSig.get());



	m_LightManager = make_unique<LightManager>();


	// 메인 패쓰 추가
	m_MainPass = AddPassData();

	// 카메라 설치
	auto MainCam = make_unique<Camera>();
	MainCam->SetLens(4.0f / XM_PI,
		(float)m_SceneConfig.Width / (float)m_SceneConfig.Height,
		core->GetSettingDesc().NearZ,
		core->GetSettingDesc().FarZ);

	MainCam->UpdateViewMatrix();
	m_MainCamera = MainCam.get();
	m_Cameras.push_back(move(MainCam));


	// 장면 구 설정( 나중에 크기 조절 가능하게 ) 나중에 모든 오브젝트를 훑어서 자동 으로 늘렷다 줄엿다.
	m_SceneSphere.Center = { 0.0f,0.0f, 0.0f };
	m_SceneSphere.Radius = sqrt(500.0f * 500.0f + 500.0f * 500.0f);
}
void Scene::BuildScene()
{
	auto CommandList = m_CmdListManager->GetCommandList(0);

	// 씬 데이터 빌드
	m_SceneData->BuildSceneData(m_SceneConfig.Width, m_SceneConfig.Height);
	m_LightManager->BuildLight(m_CommonShaderRootSig.get());
	// ssao 빌드
	m_SSAO = make_unique<SSAO>();
	m_SSAO->BuildSSAO(m_SceneConfig.Width, m_SceneConfig.Height,
		CommandList, m_CommonShaderRootSig.get());
	m_Blur = make_unique<BlurFilter>(m_SceneConfig.Width, m_SceneConfig.Height);
	// 오브젝트 분리
	for (auto& obj : m_ObjectMems)
	{
		EPSOMode Mode = obj->GetMaterial()->GetDesc()->Mode;
		EObjType Type = obj->GetType();

		m_ObjectArray[Type][Mode].push_back(obj);
	}
	// 텍스쳐 추가
	for (auto& data : m_TextureDatas)
	{
		m_ResourceManager->AddTexture(CommandList, data.Path, data.Type);
	}
	// 각종 포스트 프로세싱 빌드
	m_ResourceManager->BuildResourceManager(CommandList, m_CommonShaderRootSig.get());


	m_FrameResourceManager->BuildFrameResource(m_DxCore->Device(), 
		(UINT)max(1, m_ResourceManager->PassDataSize()), (UINT)max(1, m_ResourceManager->JGRCObjectSize() + 1),
		(UINT)max(1, m_ResourceManager->JGMaterialSize()),
		(UINT)max(1, m_LightManager->Size()));
}
void Scene::OnReSize(UINT width, UINT height)
{
	m_SceneData->ReSize(width, height);
	m_SSAO->OnReSize(width, height);
}
void Scene::Update(const GameTimer& gt)
{
	// 씬 설정 업데이트
	m_SceneConfig = m_DxCore->GetSettingDesc();

	// 메인 카메라 업데이트
	m_MainCamera->UpdateViewMatrix();

	m_FrameResourceManager->FrameResourcePerFrame(m_GCS->GetFence());

	auto CurrFrameResource = m_FrameResourceManager->CurrentFrameResource();
	m_SSAO->Update(CurrFrameResource);
	// 오브젝트 업데이트
	for (auto& obj : m_ObjectMems)
		obj->Update(gt, CurrFrameResource);
	// 머터리얼 업데이트
	for (auto& mat : m_MaterialMems)
		mat->Update(CurrFrameResource);

	// 라이트 매니저 
	m_LightManager->Update(CurrFrameResource);

	// 메인 패스 업데이트
	MainPassUpdate(gt);

}
void Scene::Draw()
{
	// 각 명령 리스트 및 메모리 초기화
	auto CommandList = m_CmdListManager->GetCommandList(0);
	auto CurrFrameResource = m_FrameResourceManager->CurrentFrameResource();
	auto CmdLisAlloc = CurrFrameResource->CmdListAlloc;
	ThrowIfFailed(CmdLisAlloc->Reset());
	ThrowIfFailed(CommandList->Reset(CmdLisAlloc.Get(), nullptr));

	/*
	Srv Heap, RootSignature, TextureArray, Material 등 명령리스트에 등록 */
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_ResourceManager->SrvHeap() };
	CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);



	CommandList->SetGraphicsRootSignature(m_CommonShaderRootSig->GetRootSignature());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ECommonShaderSlot::srvTexture,
		m_ResourceManager->SrvHeap()->GetGPUDescriptorHandleForHeapStart());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ECommonShaderSlot::srvCubeMap,
		m_ResourceManager->GetGPUCubeMapHandle());
	auto MatCB = CurrFrameResource->MaterialCB->Resource();
	CommandList->SetGraphicsRootShaderResourceView((UINT)ECommonShaderSlot::sbMaterialData,
		MatCB->GetGPUVirtualAddress());
	auto LightCB = CurrFrameResource->LightCB->Resource();
	CommandList->SetGraphicsRootShaderResourceView((UINT)ECommonShaderSlot::sbLightData,
		LightCB->GetGPUVirtualAddress());


	m_SSAO->Draw(CurrFrameResource, CommandList);





	CommandList->SetGraphicsRootSignature(m_CommonShaderRootSig->GetRootSignature());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ECommonShaderSlot::srvTexture,
		m_ResourceManager->SrvHeap()->GetGPUDescriptorHandleForHeapStart());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ECommonShaderSlot::srvCubeMap,
		m_ResourceManager->GetGPUCubeMapHandle());
	CommandList->SetGraphicsRootShaderResourceView((UINT)ECommonShaderSlot::sbMaterialData,
		MatCB->GetGPUVirtualAddress());
	CommandList->SetGraphicsRootShaderResourceView((UINT)ECommonShaderSlot::sbLightData,
		LightCB->GetGPUVirtualAddress());


	m_LightManager->DrawShadowMap(CommandList, CurrFrameResource);
	


	for (auto& objarr : m_ObjectArray)
	{
		for (auto& obj : objarr.second[EPSOMode::DEFAULT])
		{
			obj->CubeMapDraw(CurrFrameResource, CommandList);
		}
	}



	// 메인 패쓰 등록
	CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerPass,
		MainPassHandle());
	m_SceneData->SceneDataExtract(CurrFrameResource, CommandList);


	// SwapChain에 그리기 시작
	m_ScreenManager->ReadyScreen(m_Name, CommandList);

	CommandList->SetPipelineState(m_ScenePSO.Get());
	CommandList->IASetVertexBuffers(0, 0, nullptr);
	CommandList->IASetIndexBuffer(nullptr);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawInstanced(6, 1, 0, 0);
	//m_Blur->Execute(m_DxCore->CurrentBackBuffer(), CommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, 1);
	// 마무리
	m_ScreenManager->OutputScreen(m_Name, CommandList);

	CurrFrameResource->Fence = m_GCS->GetOffset();
}
void Scene::SceneObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode, bool bDebug)
{
	for (auto& objarr : m_ObjectArray)
	{
		for (auto& obj : objarr.second[EPSOMode::DEFAULT])
		{
			obj->Draw(CurrFrameResource, CommandList, Mode);
		}
		if (bDebug)
		{
			for (auto& obj : objarr.second[EPSOMode::DEBUG])
			{
				obj->Draw(CurrFrameResource, CommandList, Mode);
			}
		}
	}

}
void Scene::SceneStaticObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode)
{
	for (auto& obj : m_ObjectArray[EObjType::Static][EPSOMode::DEFAULT])
	{
		obj->Draw(CurrFrameResource, CommandList, Mode);
	}
}
void Scene::SceneDynamicObjectDraw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, EObjRenderMode Mode)
{
	for (auto& obj : m_ObjectArray[EObjType::Dynamic][EPSOMode::DEFAULT])
	{
		obj->Draw(CurrFrameResource, CommandList, Mode);
	}
}
JGRCObject* Scene::CreateObject(JGMaterial* mat, JGBaseMesh* mesh, const string& meshname, EObjType Type)
{
	JGRCObject* result = m_ResourceManager->AddJGRCObject(mat, mesh, meshname, Type);
	m_ObjectMems.push_back(result);
	return result;
}
JGRCObject* Scene::CreateSkyBox(const std::wstring& texturepath)
{
	bool result = false;
	//스카이 박스 머터리얼 및 메쉬 미리 생성
	for (auto& path : m_TextureDatas)
		if (texturepath == path.Path)
		{
			result = true;
			break;
		}
	if (!result)
		return nullptr;


	// 머터리얼 생성
	MaterialDesc matDesc;
	matDesc.Mode = EPSOMode::SKYBOX;
	matDesc.ShaderPath = global_sky_hlsl_path;
	matDesc.Name = string().assign(m_SkyShaderPath.begin(), m_SkyShaderPath.end());
	JGMaterial* SkyMat = AddMaterial(matDesc);
	SkyMat->SetTexture(ETextureSlot::Diffuse, texturepath);
	// 메쉬 생성
	JGStaticMesh* SkyMesh = AddStaticMesh();
	SkyMesh->AddBoxArg("Scene_SkyBox_Default_Mesh", 1.0f, 1.0f, 1.0f, 0);
	// 오브젝트 생성
	JGRCObject* obj = CreateObject(SkyMat, SkyMesh,"Scene_SkyBox_Default_Mesh" );
	obj->SetScale(5000.0f, 5000.0f, 5000.0f);
	return obj;
}
void   Scene::AddDebugBox(JGRCObject* obj, const XMFLOAT3& color, float thickness)
{
	auto debugBox = make_unique<DebugBox>();
	debugBox->BindingObject(obj, color, thickness);
	m_DebugMems.push_back(move(debugBox));
}
JGMaterial* Scene::AddMaterial(const MaterialDesc& Desc)
{
	JGMaterial* mat  = m_ResourceManager->AddMaterial(Desc);
	m_MaterialMems.push_back(mat);
	return mat;
}
JGStaticMesh*   Scene::AddStaticMesh()
{
	return m_ResourceManager->AddStaticMesh();
}
JGSkeletalMesh* Scene::AddSkeletalMesh()
{
	return m_ResourceManager->AddSkeletalMesh();
}
string Scene::AddAnimation(const string& path)
{
	if (m_Animations.find(path) != m_Animations.end())
		return string("None");


	auto anim = make_unique<JGAnimation>();
	ResourceReader reader(path, *anim);
	JGAnimation* result = anim.get();

	m_AnimationMems.push_back(move(anim));
	m_Animations[path] = result;
	return path;
}
Camera*     Scene::AddCamera()
{
	auto cam = make_unique<Camera>();
	Camera* result = cam.get();
	m_Cameras.push_back(move(cam));
	return result;
}
PassData* Scene::AddPassData()
{
	return m_ResourceManager->AddPassData();
}
JGLight*  Scene::AddLight(ELightType type, ELightExercise extype)
{
	return m_LightManager->AddLight(type, extype);
}
void Scene::AddTexture(const wstring& TexturePath, ETextureType type)
{
	m_TextureDatas.push_back(TexturePack{ move(TexturePath), nullptr, type });
}
void  Scene::SettingDefaultSceneBuffer(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource)
{
	CommandList->SetGraphicsRootSignature(m_CommonShaderRootSig->GetRootSignature());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ECommonShaderSlot::srvTexture,
		m_ResourceManager->SrvHeap()->GetGPUDescriptorHandleForHeapStart());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ECommonShaderSlot::srvCubeMap,
		m_ResourceManager->GetGPUCubeMapHandle());
	auto MatCB = CurrFrameResource->MaterialCB->Resource();
	CommandList->SetGraphicsRootShaderResourceView((UINT)ECommonShaderSlot::sbMaterialData,
		MatCB->GetGPUVirtualAddress());
	auto LightCB = CurrFrameResource->LightCB->Resource();
	CommandList->SetGraphicsRootShaderResourceView((UINT)ECommonShaderSlot::sbLightData,
		LightCB->GetGPUVirtualAddress());
}
JGAnimation* Scene::GetAnimation(const string& name)
{
	if (m_Animations.find(name) == m_Animations.end())
		return nullptr;
	return m_Animations[name];
}
SceneData* Scene::GetSceneData() const
{
	return m_SceneData.get();
}
CommonShaderRootSignature* Scene::GetRootSig()
{
	return m_CommonShaderRootSig.get();
}
CommonSkinnedShaderRootSignature* Scene::GetSkinnedRootSig()
{
	return m_CommonSkinnedShadeRootSig.get();
}
D3D12_GPU_VIRTUAL_ADDRESS Scene::MainPassHandle()
{
	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(cbPassConstant));
	D3D12_GPU_VIRTUAL_ADDRESS Handle = m_FrameResourceManager->CurrentFrameResource()->PassCB->Resource()->GetGPUVirtualAddress();
	Handle += m_MainPass->PassCBIndex * passCBByteSize;
	return Handle;
}
std::vector<JGRCObject*>& Scene::GetArray(EObjType objType, EPSOMode mode)
{
	return m_ObjectArray[objType][mode];
}
void Scene::MainPassUpdate(const GameTimer& gt)
{
	auto CurrFrameResource = m_FrameResourceManager->CurrentFrameResource();
	XMMATRIX view = m_MainCamera->GetView();
	XMMATRIX proj = m_MainCamera->GetProj();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX viewProjTex = XMMatrixMultiply(viewProj, T);
	// 기본 행렬 
	XMStoreFloat4x4(&m_MainPass->Data.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&m_MainPass->Data.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&m_MainPass->Data.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&m_MainPass->Data.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&m_MainPass->Data.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&m_MainPass->Data.InvViewProj, XMMatrixTranspose(invViewProj));
	XMStoreFloat4x4(&m_MainPass->Data.ViewProjTex, XMMatrixTranspose(viewProjTex));

	// 카메라 위치
	m_MainPass->Data.EyePosW = m_MainCamera->GetPosition3f();


	// 렌더 타겟 사이즈
	m_MainPass->Data.RenderTargetSize = XMFLOAT2((float)m_SceneConfig.Width, (float)m_SceneConfig.Height);
	m_MainPass->Data.InvRenderTargetSize = XMFLOAT2(1 / (float)m_SceneConfig.Width, 1 / (float)m_SceneConfig.Height);

	//
	m_MainPass->Data.TotalTime = gt.TotalTime();
	m_MainPass->Data.DeltaTime = gt.DeltaTime();
	// 씬 인덱스
	m_MainPass->Data.WorldPosSceneIndex = m_SceneData->GetWorldPosIndex();
	m_MainPass->Data.AlbedoSceneIndex = m_SceneData->GetAlbedoIndex();
	m_MainPass->Data.NormalSceneIndex = m_SceneData->GetNormalIndex();
	m_MainPass->Data.DepthSceneIndex = m_SceneData->GetDepthIndex();
	m_MainPass->Data.MatSceneIndex = m_SceneData->GetMatIndex();
	m_MainPass->Data.SSAOTexutreIndex = m_SSAO->GetShaderIndex();
	if(m_MainSkyBox)
		m_MainPass->Data.SkyBoxIndex = m_ResourceManager->GetCubeTextureShaderIndex(
			m_MainSkyBox->GetMaterial()->GetTexturePath(ETextureSlot::Diffuse));
	auto PassCB = CurrFrameResource->PassCB.get();
	PassCB->CopyData(m_MainPass->PassCBIndex, m_MainPass->Data);
}
void Scene::InitStaticMemberVar()
{
	CommonData(m_DxCore, this, m_ResourceManager, m_ScreenManager);
	ShadowMap(2048, 2048);
}