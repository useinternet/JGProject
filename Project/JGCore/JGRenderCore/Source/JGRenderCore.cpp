#include"JGRenderCore.h"
#include"DirectX/DirectX.h"
#include"Common/JGInitConfig.h"
#include"SceneRenderSystem/JGSRSystem.h"
#include"SceneRenderSystem/SRSRenderTarget.h"
#include"SceneRenderSystem/SRSScene.h"
#include"BufferSystem/JGBufferManager.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObjectManager.h"
#include"MaterialSystem/Texture/TextureManager.h"
#include"MaterialSystem/MaterialSystem.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderCompiler.h"
#include"DirectX/JGViewport.h"
#include"MaterialSystem/Shader/ShaderAnalyze/ShaderAnalyzer.h"
#include"MaterialSystem/MaterialReader.h"
#include"MaterialSystem/Material.h"
#include"MaterialSystem/Mesh/Mesh.h"
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{

}
JGRenderCore::~JGRenderCore()
{
	delete scene;
	delete RTT;
	TextureManager::Release();
	MaterialSystem::Release();
	ShaderObjectManager::Release();
	JGBufferManager::Release();
	DirectX::Release();
}
void JGRenderCore::Init(HWND hWnd)
{
	JGLOGINIT("JGRenderCoreLog.log");
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
	MaterialSystem::GetInstance();
	TextureManager::GetInstance();
	ShaderCompiler::hWnd = hWnd;

	DrawEvent e;
	e.rank = 0;
	e.func = bind(&JGRenderCore::SceneDraw,this);
	m_DX->RegistorDrawEvent(e);
	//// 셰이더 분석
	ShaderAnalyzer Sample;
	Sample.Analyze(Game::path / "HLSL/CommonShader_vs.hlsl", EShaderType::Vertex);
	Sample.Analyze(Game::path / "HLSL/CommonShader_ps.hlsl", EShaderType::Pixel);
	Sample.OutputShaderData(Game::path / "SampleMaterial");
	
	ShaderAnalyzer GroundAyz;
	GroundAyz.Analyze(Game::path / "HLSL/SRS_vs.hlsl", EShaderType::Vertex);
	GroundAyz.Analyze(Game::path / "HLSL/SRS_ps.hlsl", EShaderType::Pixel);
	GroundAyz.OutputShaderData(Game::path / "SamplerGround");

	// 메쉬 생성
	rabbit = make_unique<Mesh>();
	rabbit->LoadModel(Game::path / "bunny.txt", false);
	ground = make_unique<Mesh>();
	ground->LoadModel(Game::path / "cube.txt");
	// 머터리얼 읽기
	MaterialReader rd;
	mt = rd.Read(Game::path / "SampleMaterial.material");
	mt->SetMesh(rabbit.get());
	plane = rd.Read(Game::path / "SamplerGround.material");
	plane->SetMesh(ground.get());
	plane->AddTexturePath(Game::path / "Texture/T_Water_M.png");
	plane->AddTexturePath(Game::path / "Texture/T_Water_N.png");

	// 실험 렌더 타겟
	RTT = new SRSRenderTarget;
	RTT->CreateSRSRenderTarget(1920, 1080);

	// 실험 씬
	scene = new SRSScene;
	scene->CreateScene(1920, 1080);
}
void JGRenderCore::Draw()
{
	modelDraw();
	m_DX->Draw();

}
void JGRenderCore::SceneDraw()
{
	scene->Render(RTT);
}
void JGRenderCore::modelDraw()
{
	RTT->BindingRenderTarget();
	RTT->ClearRenderTarget();
	/////////////////////////// 토끼 /////////////////////////////////////
	static real yaw = 0;
	yaw += 0.001f;
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}
	rabbitDraw(yaw);
	groundDraw(yaw);



	m_DX->SetDefautRenderTarget();
}
void JGRenderCore::rabbitDraw(float yaw)
{
	jgVec3 CameraPos(0.0f, 5.0f, -10.0f);

	// 월드 매트릭스
	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	worldMatrix.rotationY(yaw);
	worldMatrix.transpose();

	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(CameraPos, jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	viewMatrix.transpose();


	jgMatrix4x4 projectionMatrix = m_DX->GetViewport()->GetProjectionMatrix();
	projectionMatrix.transpose();


	real color[4] = { 1.0f,1.0f,1.0f,1.0f };

	mt->SetParam("worldMatrix", &worldMatrix);

	worldMatrix = projectionMatrix * viewMatrix * worldMatrix;
	mt->SetParam("wvpMatrix", &worldMatrix);
	mt->SetParam("AmbientColor", color);
	mt->Render();
}
void JGRenderCore::groundDraw(float yaw)
{
	jgVec3 CameraPos(0.0f, 5.0f, -10.0f);
	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	jgMatrix4x4 transMatrix;
	transMatrix.translation(jgVec3(0.0, -1.0f, 0.0f));
	jgMatrix4x4 scaleMatrix;
	scaleMatrix.scaling(5.0f, 0.2f, 5.0);
	jgMatrix4x4 rotationMatrix;
	rotationMatrix.rotationY(yaw);

	worldMatrix = worldMatrix * scaleMatrix * rotationMatrix * transMatrix;
	worldMatrix.transpose();
	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(CameraPos, jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	viewMatrix.transpose();


	jgMatrix4x4 projectionMatrix = m_DX->GetViewport()->GetProjectionMatrix();
	projectionMatrix.transpose();

	


	plane->SetParam("worldMatrix", &worldMatrix);
	worldMatrix = projectionMatrix * viewMatrix * worldMatrix;
	plane->SetParam("wvpMatrix", &worldMatrix);
	plane->Render();
}