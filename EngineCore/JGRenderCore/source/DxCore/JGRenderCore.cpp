#include"JGRenderCore.h"
#include"DxDevice.h"
#include"Data/Scene.h"
#include"ResourceManagement/ResourceExtracter.h"
#include"ResourceManagement/ResourceReader.h"
#include"ResourceManagement/DataManager.h"
#include"RootSignatureManager.h"
#include"ScreenManager.h"
#include"CommandListManager.h"
#include"GpuCpuSynchronizer.h"
#include"Data/CommonData.h"
#include"Data/LightManager.h"
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{
	m_GCS             = make_unique<GpuCpuSynchronizer>();
	m_CmdListManager  = make_unique<CommandListManager>();
	m_ScreenManager   = make_unique<ScreenManager>();
	m_DataManager     = make_unique<DataManager>();
	m_ResourceManager = make_unique<ResourceManager>();
	m_RootSigManager  = make_unique<RootSignatureManager>();
	m_FrameResourceManager = make_unique<EngineFrameResourceManager>();
}
bool JGRenderCore::CreateCore()
{
	m_DxDevice = make_unique<DxDevice>();
	if (!m_DxDevice->CreateDevice())
	{
		// 예외 처리
		return false;
	}
	m_CmdListManager->CreateManager(m_DxDevice->Get());
	m_GCS->CreateSynchronizer(m_DxDevice->Get());
	m_ScreenManager->CreateManager(
		m_DxDevice->Factory(), 
		m_ResourceManager.get(), 
		m_GCS.get(),
		m_CmdListManager.get());
	m_ResourceManager->Init(m_DxDevice.get());
	m_RootSigManager->CreateManager(m_DxDevice->Get());

	m_CmdListManager->AddCommandList();
	CommonData(
		m_DxDevice.get(),
		nullptr, 
		m_ResourceManager.get(),
		m_DataManager.get(),
		m_ScreenManager.get(),
		m_CmdListManager.get(), 
		m_GCS.get(),
		m_RootSigManager.get(),
		m_FrameResourceManager.get());
	return true;
}
IF_Scene JGRenderCore::CreateScene(const string& SceneName, const SceneConfig& set)
{
	if (m_Scene)
		return IF_Scene(m_Scene.get());

	m_Scene = make_unique<Scene>(SceneName, set);
	m_ScreenManager->AddSwapChain(SceneName, set.hWnd, set, m_CmdListManager.get(), 0);
	CommonData::RegisterScene(m_Scene.get());
	return IF_Scene(m_Scene.get());
}
IF_Scene JGRenderCore::GetScene()
{
	return IF_Scene(m_Scene.get());
}
void JGRenderCore::Build(const GameTimer& gt)
{
	m_Scene->BuildScene();
	m_ResourceManager->BuildResourceManager(m_CmdListManager->GetCommandList(0));
	m_DataManager->Build();
	m_FrameResourceManager->BuildFrameResource(m_DxDevice->Get(),
		(UINT)max(1, m_DataManager->PassDataCount()), (UINT)max(1, m_DataManager->ObjectCount() + 1),
		(UINT)max(1, m_DataManager->MaterialDataCount()),
		(UINT)max(1, m_Scene->GetLightManager()->Size()));
	// 명령 리스트&큐  초기화
	m_CmdListManager->ExcuteCommandLists();


	for (int i = 0; i < CPU_FRAMERESOURCE_NUM; ++i)
	{
		Update(gt);
		Draw();
	}
}
void JGRenderCore::Update(const GameTimer& gt)
{
	m_FrameResourceManager->FrameResourcePerFrame(m_GCS->GetFence());
	m_Scene->Update(gt);
}
void JGRenderCore::ReSize(int width, int height)
{
	// 추후 구현
}
void JGRenderCore::Draw()
{
	m_Scene->Draw();
	m_FrameResourceManager->CurrentFrameResource()->Fence = m_GCS->GetOffset();
}