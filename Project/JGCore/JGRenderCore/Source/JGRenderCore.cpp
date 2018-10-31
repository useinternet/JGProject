#include"JGRenderCore.h"
#include"DirectX/DirectX.h"
#include"Common/JGInitConfig.h"
#include"BufferSystem/JGBufferManager.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObjectManager.h"
#include"MaterialSystem/Shader/ShaderTool/SamplerState.h"
#include"TestClass/TestModel.h"
#include"TestClass/TestShader.h"
#include"DirectX/JGViewport.h"
#include"MaterialSystem/Shader/ShaderAnalyze/ShaderAnalyzer.h"
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{

}
JGRenderCore::~JGRenderCore()
{
	delete shader;
	model->Shutdown();
	delete model;

	SamplerState::Release();
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

	m_DX = DirectX::GetInstance();
	m_DX->Init(config);
	m_DX->SetDefaultDirectState();

	JGBufferManager::GetInstance();
	ShaderObjectManager::GetInstance();
	SamplerState::GetInstance();


	model = new TestModel;
	model->Initialize(m_DX->GetDevice(), "cube.txt", nullptr);
	shader = new TestShader;
	shader->Initialize(m_DX->GetDevice(), hWnd);

	DrawEvent e;
	e.rank = 0;
	e.func = bind(&JGRenderCore::modelDraw,this);
	m_DX->RegistorDrawEvent(e);


	//ShaderAnalyzer s;
	//s.Analyze("./HLSL/CommonShader_vs.hlsl", EShaderType::Vertex);
	//ShaderAnalyzer p;
	//p.Analyze("./HLSL/CommonShader_ps.hlsl", EShaderType::Pixel);
}
void JGRenderCore::Draw()
{
	m_DX->Draw();
}
void JGRenderCore::modelDraw()
{
	static real yaw = 0;
	yaw += 0.001f;
	model->Render(m_DX->GetContext());

	//
	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	worldMatrix.rotationY(yaw);



	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(jgVec3(0.0f, 0.0f, -4.0f), jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));

	shader->Render(m_DX->GetContext(), model->GetIndexCount(), worldMatrix, viewMatrix,
		m_DX->GetViewport()->GetProjectionMatrix(), nullptr);
}