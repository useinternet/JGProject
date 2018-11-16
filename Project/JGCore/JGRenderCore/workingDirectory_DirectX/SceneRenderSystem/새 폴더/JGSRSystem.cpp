#include"JGSRSystem.h"
#include"DirectX/DirectX.h"
#include"SRSRenderTarget.h"
#include"SRSScene.h"
#include"MaterialSystem/Material.h"
using namespace std;
using namespace JGRC;

JGSRSystem::JGSRSystem()
{
	m_Dx = DirectX::GetInstance();
	m_Scene = make_unique<SRSScene>();
	m_SRSRTT = make_unique<SRSRenderTarget>();
}
JGSRSystem::~JGSRSystem()
{

}
bool JGSRSystem::Init(const int width, const int height)
{
	m_Scene->CreateScene(width, height);
	bool result = m_SRSRTT->CreateSRSRenderTarget(width, height);
	return result;
}
void JGSRSystem::push_Material(Material* mt)
{
	m_matQue.push(mt);
}
void JGSRSystem::Draw()
{
	m_SRSRTT->BindingRenderTarget();
	m_SRSRTT->ClearRenderTarget();
	while (!m_matQue.empty())
	{
		Material* mt = m_matQue.front();
		if (mt)
		{
			mt->Render();
		}
		m_matQue.pop();
	}
	//m_Dx->SetDefautRenderTarget();
}
void JGSRSystem::SceneDraw()
{
	//m_Dx->BeginDraw();
	//m_Scene->Render(m_SRSRTT.get());
	//m_Dx->EndDraw();
}
