#include"JGRenderCore.h"
#include"DirectX/DirectX.h"
#include"Common/JGRCGlobalType.h"
#include"SceneRenderSystem/JGSRSystem.h"
#include"BufferSystem/JGBufferManager.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObjectManager.h"
#include"MaterialSystem/Texture/TextureManager.h"
#include"MaterialSystem/MaterialSystem.h"
#include"MaterialSystem/Material.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderCompiler.h"
#include"MaterialSystem/Shader/ShaderAnalyze/ShaderAnalyzer.h"
#include"MaterialSystem/MaterialReader.h"

//임시
#include"DirectX/JGViewport.h"
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{

}
JGRenderCore::~JGRenderCore()
{
	if (m_SRSytsem)
	{
		delete m_SRSytsem;
		m_SRSytsem = nullptr;
	}
	TextureManager::Release();
	MaterialSystem::Release();
	ShaderObjectManager::Release();
	JGBufferManager::Release();
	DirectX::Release();
}
void JGRenderCore::Init(HWND hWnd)
{
	JGInitConfig config;
	config.bFullScreen = false;
	config.FarZ = 1000.0f;
	config.NearZ = 0.1f;
	config.hWnd = hWnd;
	config.ScreenHeight = 1080;
	config.ScreenWidth = 1920;
	config.Fov = JGPI / 4.0f;
	// 다이렉트 초기화
	m_DX = DirectX::GetInstance();
	m_DX->Init(config);
	m_DX->SetDefaultDirectState();
	ShaderCompiler::hWnd = hWnd;

	m_SRSytsem = new JGSRSystem;
	m_SRSytsem->Init(config.ScreenWidth, config.ScreenHeight);
}
void JGRenderCore::Push(class Material* mt)
{
	m_SRSytsem->push_Material(mt);
}
void JGRenderCore::Draw()
{
	m_SRSytsem->Draw();
	m_SRSytsem->SceneDraw();
}
bool JGRenderCore::OutputMaterialData(string* hlslPath, EShaderType* type, const UINT Count,const string& OuputMaterial)
{
	ShaderAnalyzer ShaderAnyz;
	bool result = true;
	for (UINT i = 0; i < Count; ++i)
	{
		result = ShaderAnyz.Analyze(hlslPath[i], type[i]);
		if (!result)
		{
			return false;
		}
	}
	result = ShaderAnyz.OutputShaderData(OuputMaterial);
	if (!result)
	{
		return false;
	}
	return true;
}
Material* JGRenderCore::CreateMaterial(const string& mtPath)
{
	MaterialReader reader;
	Material* result = reader.Read(mtPath);
	if (result == nullptr)
	{
		JGLOG(log_Error, "JGRC::JGRenderCore", "Failed Read Material :" + mtPath);
	}
	return result;
}
void JGRenderCore::ManagerInit()
{
	// 각 매니저 초기화
	JGBufferManager::GetInstance();
	ShaderObjectManager::GetInstance();
	MaterialSystem::GetInstance();
	TextureManager::GetInstance();

}

jgMatrix4x4 JGRenderCore::GetProjectionMatrix()
{
	return m_DX->GetViewport()->GetProjectionMatrix();
}