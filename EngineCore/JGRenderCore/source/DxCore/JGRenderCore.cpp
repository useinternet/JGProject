#include"JGRenderCore.h"
#include"DxCore.h"
#include"Data/Scene.h"
#include"ResourceManagement/ResourceExtracter.h"
#include"ResourceManagement/ResourceReader.h"
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{

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
	ThrowIfFailed(m_DxCore->CommandList()->Reset(m_DxCore->CommandAllocator(), nullptr));
	return true;
}
IF_Scene JGRenderCore::CreateScene(const string& SceneName)
{
	if (m_Scenes.find(SceneName) != m_Scenes.end())
		return IF_Scene(nullptr);

	auto s = make_unique<Scene>(m_DxCore.get());
	IF_Scene result(s.get());
	m_Scenes[SceneName] = s.get();
	m_SceneMems.push_back(move(s));
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

	ThrowIfFailed(m_DxCore->CommandList()->Close());
	ID3D12CommandList* cmdsList[] = { m_DxCore->CommandList() };
	m_DxCore->CommandQueue()->ExecuteCommandLists(_countof(cmdsList), cmdsList);


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