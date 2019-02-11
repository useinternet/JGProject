#include"JGRenderCore.h"
#include"DxCore.h"
#include"Data/Scene.h"
#include"ResourceManagement/ResourceExtracter.h"
#include"ResourceManagement/ResourceReader.h"
#include"ScreenManager.h"
#include"CommandListManager.h"
#include"GpuCpuSynchronizer.h"
#include"Data/CommonData.h"
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{
	m_Synchronizer    = make_unique<GpuCpuSynchronizer>();
	m_CmdListManager  = make_unique<CommandListManager>();
	m_ScreenManager   = make_unique<ScreenManager>();
	m_ResourceManager = make_unique<ResourceManager>();
}
JGRenderCore::~JGRenderCore()
{
}

bool JGRenderCore::Init(const DxSetting& set)
{
	m_DxCore = make_unique<DxCore>(set);
	if (!m_DxCore->InitDirect3D())
	{
		// 예외 처리
		return false;
	}
	
	//ThrowIfFailed(m_DxCore->CommandList()->Reset(m_DxCore->CommandAllocator(), nullptr));
	m_CmdListManager->CreateManager(m_DxCore->Device());
	m_Synchronizer->CreateSynchronizer(m_DxCore->Device());
	m_ScreenManager->CreateManager(
		m_DxCore->Factory(), 
		m_ResourceManager.get(), 
		m_Synchronizer.get(),
		m_CmdListManager.get());
	m_ResourceManager->Init(m_DxCore.get());


	m_CmdListManager->AddCommandList();
	return true;
}
IF_Scene JGRenderCore::CreateScene(const string& SceneName, const DxSetting& set)
{
	if (m_Scenes.find(SceneName) != m_Scenes.end())
		return IF_Scene(nullptr);

	auto s = make_unique<Scene>(
		SceneName,
		m_DxCore.get(), 
		m_ResourceManager.get(),
		m_CmdListManager.get(),
		m_ScreenManager.get(),
		m_Synchronizer.get());
	IF_Scene result(s.get());
	m_Scenes[SceneName] = s.get();
	m_SceneMems.push_back(move(s));


	//
	m_ScreenManager->AddSwapChain(SceneName, set.hWnd, set, m_CmdListManager.get(), 0);

	return result;
}
IF_Scene JGRenderCore::GetScene(const string& SceneName)
{
	if (m_Scenes.find(SceneName) == m_Scenes.end())
		return IF_Scene(nullptr);
	return m_Scenes[SceneName];
}
void JGRenderCore::Build(const GameTimer& gt)
{
	for (auto& s : m_SceneMems)
	{
		s->BuildScene();
	}
	// 명령 리스트&큐  초기화
	m_CmdListManager->ExcuteCommandLists();
	//ThrowIfFailed(m_DxCore->CommandList()->Close());
	//ID3D12CommandList* cmdsList[] = { m_DxCore->CommandList() };
	//m_DxCore->CommandQueue()->ExecuteCommandLists(_countof(cmdsList), cmdsList);


	for (auto& s : m_SceneMems)
	{
		for (int i = 0; i < CPU_FRAMERESOURCE_NUM; ++i)
		{
			s->Update(gt);
			s->Draw();
		}
	}
}
void JGRenderCore::Update(const GameTimer& gt)
{
	for (auto& s : m_SceneMems)
	{
		s->Update(gt);
	}
}
void JGRenderCore::ReSize(int width, int height)
{
	// 추후 구현
}
void JGRenderCore::Draw()
{
	for (auto& s : m_SceneMems)
	{
		s->Draw();
	}
}