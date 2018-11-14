#include"JGRenderCore.h"
#include"DirectX/DirectX.h"
#include"DirectX/DxWindow.h"
#include"MaterialSystem/Shader/HlslEditor.h"
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{

}
JGRenderCore::~JGRenderCore()
{
	DirectX::Release();
	string str = to_string(JGRCObject::GetCount());
	JGLOG(log_Info, "Count", str);
}
void JGRenderCore::Init()
{
	m_DX = DirectX::GetInstance();
	m_DX->Init();
}
DxWindow* JGRenderCore::AddDxWindow(const DxWinConfig& config) const
{
	return m_DX->AddDxWindow(config);
}
DxWindow* JGRenderCore::GetDxWindow(HWND hWnd)
{
	return m_DX->GetWindow(hWnd);
}
HlslEditor* JGRenderCore::GetHlslEditor(const EShaderType type, const std::string& path) const
{
	HlslEditor* edit = m_DX->CreateObject<HlslEditor>();
	edit->RegisterHlslPath(type, path);
	return edit;
}
void JGRenderCore::Draw()
{
	m_DX->Draw();
}