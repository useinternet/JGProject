#include"JGRenderCore.h"
#include"DirectX/DirectX.h"
#include"Common/JGInitConfig.h"
#include"BufferSystem/JGBufferManager.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObjectManager.h"
#include"MaterialSystem/Texture/TextureManager.h"
#include"MaterialSystem/MaterialSystem.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderCompiler.h"
#include"DirectX/JGViewport.h"
#include"MaterialSystem/Shader/ShaderAnalyze/ShaderAnalyzer.h"
#include"MaterialSystem/MaterialReader.h"
#include"MaterialSystem/Material.h"
using namespace JGRC;
using namespace std;

JGRenderCore::JGRenderCore()
{

}
JGRenderCore::~JGRenderCore()
{
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
	e.func = bind(&JGRenderCore::modelDraw,this);
	m_DX->RegistorDrawEvent(e);
	ShaderAnalyzer Sample;
	Sample.Analyze("./HLSL/CommonShader_vs.hlsl", EShaderType::Vertex);
	Sample.Analyze("./HLSL/CommonShader_ps.hlsl", EShaderType::Pixel);
	Sample.OutputShaderData("SampleMaterial");

	ShaderAnalyzer GroundAyz;
	GroundAyz.Analyze("./HLSL/Ground_vs.hlsl", EShaderType::Vertex);
	GroundAyz.Analyze("./HLSL/Ground_ps.hlsl", EShaderType::Pixel);
	GroundAyz.OutputShaderData("SamplerGround");
	MaterialReader rd;
	mt = rd.Read("SampleMaterial.material");
	mt->LoadModel("bunny.txt",false);
	plane = rd.Read("SamplerGround.material");
	plane->LoadModel("cube.txt",true);
	plane->AddTexturePath("./Texture/T_CobbleStone_Rough_D.png");
	plane->AddTexturePath("./Texture/T_CobbleStone_Rough_N.png");
}
void JGRenderCore::Draw()
{
	m_DX->Draw();
}
void JGRenderCore::modelDraw()
{
	/////////////////////////// Åä³¢ /////////////////////////////////////
	static real yaw = 0;
	yaw += 0.001f;
	jgVec3 CameraPos(0.0f, 5.0f, -10.0f);
	//
	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	worldMatrix.rotationY(yaw);
	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(CameraPos, jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));

	worldMatrix.transpose();
	viewMatrix.transpose();
	jgMatrix4x4 projectionMatrix = m_DX->GetViewport()->GetProjectionMatrix();
	projectionMatrix.transpose();

	mt->SetParam("worldMatrix", &worldMatrix);
	mt->SetParam("viewMatrix", &viewMatrix);
	mt->SetParam("projectionMatrix", &projectionMatrix);
	mt->SetParam("cameraPosition", &CameraPos);

	// ÇÈ¼¿ Á¤º¸ ¼ÂÆÃ
	jgVec4 Color(0.15f, 0.15f, 0.15f, 1.0f);
	mt->SetParam("ambientColor", &Color);
	Color.set(0.5f, 0.5f, 0.5f, 1.0f);
	mt->SetParam("diffuseColor", &Color);
	jgVec3 d(0.0f, -1.0f, 1.0f);
	mt->SetParam("lightDirection", &d);
	real SP = 32.0f;
	mt->SetParam("specularPower", &SP);
	Color.set(1.0f, 0.0f, 1.0f, 1.0f);
	mt->SetParam("specularColor", &Color);
	mt->Render();
	/////////////////////////// ///////////////////////////////////// 
	/////////////////////////// /////////////////////////////////////
	/////////////////////////// »óÀÚ /////////////////////////////////////

	worldMatrix.clear();
	worldMatrix.identity();
	jgMatrix4x4 transMatrix;
	transMatrix.translation(jgVec3(0.0, -1.0f, 0.0f));
	jgMatrix4x4 scaleMatrix;
	scaleMatrix.scaling(5.0f, 0.2f, 5.0);
	jgMatrix4x4 rotationMatrix;
	rotationMatrix.rotationY(yaw);
	
	worldMatrix = worldMatrix * scaleMatrix * rotationMatrix * transMatrix;
	

	worldMatrix.transpose();

	plane->SetParam("worldMatrix", &worldMatrix);
	plane->SetParam("viewMatrix", &viewMatrix);
	plane->SetParam("projectionMatrix", &projectionMatrix);

	Color.set(0.15f, 0.15f, 0.15f, 1.0f);
	plane->SetParam("ambientColor", &Color);
	Color.set(0.8f, 0.8f, 0.8f, 1.0f);
	plane->SetParam("diffuseColor", &Color);
	//d.set(1.0f, 1.0f, 1.0f);
	plane->SetParam("lightDirection", &d);
	real p = 0;
	plane->SetParam("padding", &p);

	plane->Render();
}