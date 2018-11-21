#include"JGLogicCore.h"
#include"JGRenderCore.h"

#include"MaterialSystem/Mesh/Mesh.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"MaterialSystem/Shader/ShaderReader.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"DirectX/DxWindow.h"
#include"JGRenderCore.h"
#include"DirectX/DirectX.h"

using namespace JGLC;
using namespace JGRC;
using namespace std;
JGLogicCore::~JGLogicCore()
{
	delete Cube;
	if (rabbit)
	{
		delete rabbit;
		rabbit = nullptr;
	}
	delete cam;
}
void JGLogicCore::TestInit(JGRenderCore* Rc, HWND hWnd)
{
	this->hWnd = hWnd;
	this->Rc = Rc;
	GroundInit();
	RabbitInit();
	DxWindow* wn = Rc->GetDxWindow(hWnd);
	wn->AddRenderPassEvent([this]()
	{
		this->Cube->Render();
		CubeShader->Render(Cube->getIndexCount());
		this->rabbit->Render();
		rabbitShader->Render(rabbit->getIndexCount());
	});
	cam = new Camera;
	cam->SetEye(0.0f, 3.0f, -10.0f);
	viewMatrix = cam->GetViewMatrix();
	wn->LightPass()->BindingCamera(cam);
	wn->LightPass()->AddDirectionLight();
	SpotLight* sample = wn->LightPass()->AddSpotLight();
	sample->SetColor(2.0f, 0.0f, 2.0f);
	sample->SetOuterCone(45.0f);
	sample->SetInnerConeRcp(35.0f);
	PointLight* sample2 = wn->LightPass()->AddPointLight();
	sample2->SetPosition(-3.0f, 0.0f, 0.0f);
	sample2->SetRangeRcp(0.1f);
	sample2->SetColor(0.0f, 1.0f, 1.0f);
	wn->LightPass()->DeletePointLight(sample2);
}
void JGLogicCore::TestTick()
{
	static real yaw = 0;
	yaw += 0.0001f;
	GroundRender(yaw);
	RabbitRender(yaw);
}
void JGLogicCore::GroundInit()
{
	GroundHlslInit();
	ShaderReader reader(hWnd);
	CubeShader = reader.ReadShader(Game::path / "Engine/Shader/Shader/Sample.shader");

	CubeShader->Get(EShaderType::Pixel)->AddTexture("Default_Texture", Game::path / "Engine/Texture/stone01.dds");
	CubeShader->Get(EShaderType::Pixel)->AddTexture("Normal_Texture", Game::path / "Engine/Texture/bump01.dds");
	// 설정
	Cube = new Mesh;
	Cube->LoadModel(Game::path / "cube.txt", true);
	Cube->CreateBuffer(CubeShader->Get(EShaderType::Vertex)->GetLayout());
	GroundParamInit();
}
void JGLogicCore::GroundHlslInit()
{
	////////// Vertex ///////////////
	HlslEditor* VertexHlsl = Rc->GetHlslEditor(EShaderType::Vertex, Game::path / "Engine/Shader/HLSL/StandardShader_vs.hlsl");


	// 입력 레이아웃
	InputLayout* Vertex_Il = VertexHlsl->CreateInputLayout();
	Vertex_Il->AddDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0);
	// 상수 버퍼
	CBuffer* Cbf = VertexHlsl->CreateCBuffer();
	Cbf->SetName("MatrixBuffer");
	Cbf->AddVar("worldMatrix", 0, 15);
	Cbf->AddVar("wvpMatrix", 16, 31);

	////////// Pixel ///////////////
	HlslEditor* PixelHlsl = Rc->GetHlslEditor(EShaderType::Pixel, Game::path / "Engine/Shader/HLSL/StandardShader_ps.hlsl");

	CBuffer* psCbf = PixelHlsl->CreateCBuffer();
	psCbf->SetName("Material");
	psCbf->AddVar("Ambient", 0, 2);
	psCbf->AddVar("ReflectIntensity",3, 3);
	psCbf->AddVar("SpecularColor", 4, 6);
	psCbf->AddVar("SpecularPower", 7, 7);
	psCbf->AddVar("Emissive", 8, 10);
	psCbf->AddVar("SpecularIntensity", 11, 11);


	// 텍스쳐
	Texture2D* texture = PixelHlsl->CreateTexture2D();
	texture->Add("Default_Texture");
	texture->Add("Normal_Texture");
	SamplerState* Sampler = PixelHlsl->CreateSamplerState();
	Sampler->AddDefaultWrapMode();

	ShaderWriter writer;
	writer.AddEditor(VertexHlsl);
	writer.AddEditor(PixelHlsl);
	writer.Write(Game::path / "Engine/Shader/Shader/Sample");
}
void JGLogicCore::GroundParamInit()
{
	real Ambient[3] = { 1.0f,1.0f,1.0f };
	real ReflectIntensity = 0.5f;
	real SpecularColor[3] = { 1.0f,1.0f,1.0f };
	real SpecularPower = 10.0f;
	real Emissive[3] = { 1.0f,1.0f,1.0f };
	real SpecularIntensity = 1.0f;



	CubeShader->Get(EShaderType::Pixel)->SetParam("Ambient", Ambient, 3);
	CubeShader->Get(EShaderType::Pixel)->SetParam("ReflectIntensity", &ReflectIntensity, 1);
	CubeShader->Get(EShaderType::Pixel)->SetParam("SpecularColor", SpecularColor, 3);
	CubeShader->Get(EShaderType::Pixel)->SetParam("SpecularPower", &SpecularPower, 1);
	CubeShader->Get(EShaderType::Pixel)->SetParam("Emissive", Emissive, 3);
	CubeShader->Get(EShaderType::Pixel)->SetParam("SpecularIntensity", &SpecularIntensity, 1);
}
void JGLogicCore::GroundRender(real yaw)
{
	jgMatrix4x4 rotation;
	rotation.rotationY(yaw);
	jgMatrix4x4 scale;
	scale.scaling(5.0f, 0.1f, 5.0f);
	jgMatrix4x4 translate;
	translate.translation(0.0f, -2.0f, 0.0f);

	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	worldMatrix = worldMatrix * scale * rotation * translate;

	DxWindow* wn = Rc->GetDxWindow(hWnd);
	jgMatrix4x4 projectionMatrix;

	projectionMatrix = wn->GetProjectionMatrix();

	jgMatrix4x4 mvpMatrix = worldMatrix * viewMatrix * projectionMatrix;



	worldMatrix.transpose();
	CubeShader->Get(EShaderType::Vertex)->SetParam("worldMatrix", &worldMatrix, 16);
	mvpMatrix.transpose();
	CubeShader->Get(EShaderType::Vertex)->SetParam("wvpMatrix", &mvpMatrix, 16);
}
void JGLogicCore::RabbitInit()
{
	RabiitHlslInit();
	ShaderReader reader(hWnd);
	rabbitShader = reader.ReadShader(Game::path / "Engine/Shader/Shader/Sample2.shader");

	// 설정
	rabbit = new Mesh;
	rabbit->LoadModel(Game::path / "bunny.txt", false);
	rabbit->CreateBuffer(rabbitShader->Get(EShaderType::Vertex)->GetLayout());

	RabbitParamInit();
}
void JGLogicCore::RabiitHlslInit()
{
	////////// Vertex ///////////////
	HlslEditor* VertexHlsl = Rc->GetHlslEditor(EShaderType::Vertex, Game::path / "Engine/Shader/HLSL/StandardShader2_vs.hlsl");


	// 입력 레이아웃
	InputLayout* Vertex_Il = VertexHlsl->CreateInputLayout();
	Vertex_Il->AddDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	Vertex_Il->AddDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0);
	// 상수 버퍼
	CBuffer* Cbf = VertexHlsl->CreateCBuffer();
	Cbf->SetName("MatrixBuffer");
	Cbf->AddVar("worldMatrix", 0, 15);
	Cbf->AddVar("wvpMatrix", 16, 31);

	////////// Pixel ///////////////
	HlslEditor* PixelHlsl = Rc->GetHlslEditor(EShaderType::Pixel, Game::path / "Engine/Shader/HLSL/StandardShader2_ps.hlsl");

	CBuffer* psCbf = PixelHlsl->CreateCBuffer();
	psCbf->SetName("Material");
	psCbf->AddVar("Ambient", 0, 2);
	psCbf->AddVar("ReflectIntensity", 3, 3);
	psCbf->AddVar("SpecularColor", 4, 6);
	psCbf->AddVar("SpecularPower", 7, 7);
	psCbf->AddVar("Emissive", 8, 10);
	psCbf->AddVar("SpecularIntensity", 11, 11);

	ShaderWriter writer;
	writer.AddEditor(VertexHlsl);
	writer.AddEditor(PixelHlsl);
	writer.Write(Game::path / "Engine/Shader/Shader/Sample2");
}
void JGLogicCore::RabbitParamInit()
{
	real Ambient[3] = { 1.0f,1.0f,1.0f };
	real ReflectIntensity = 0.5f;
	real SpecularColor[3] = { 1.0f,1.0f,1.0f };
	real SpecularPower = 1.0f;
	real Emissive[3] = { 1.0f,1.0f,1.0f };
	real SpecularIntensity = 0.0f;



	rabbitShader->Get(EShaderType::Pixel)->SetParam("Ambient", Ambient, 3);
	rabbitShader->Get(EShaderType::Pixel)->SetParam("ReflectIntensity", &ReflectIntensity, 1);
	rabbitShader->Get(EShaderType::Pixel)->SetParam("SpecularColor", SpecularColor, 3);
	rabbitShader->Get(EShaderType::Pixel)->SetParam("SpecularPower", &SpecularPower, 1);
	rabbitShader->Get(EShaderType::Pixel)->SetParam("Emissive", Emissive, 3);
	rabbitShader->Get(EShaderType::Pixel)->SetParam("SpecularIntensity", &SpecularIntensity, 1);
}
void JGLogicCore::RabbitRender(real yaw)
{
	jgMatrix4x4 rotation;
	rotation.rotationY(yaw);
	jgMatrix4x4 translate;
	translate.translation(0.0f, -1.5f, 0.0f);

	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	worldMatrix = worldMatrix * rotation * translate;


	DxWindow* wn = Rc->GetDxWindow(hWnd);
	jgMatrix4x4 projectionMatrix;

	projectionMatrix = wn->GetProjectionMatrix();

	jgMatrix4x4 mvpMatrix = worldMatrix * viewMatrix * projectionMatrix;

	worldMatrix.transpose();
	rabbitShader->Get(EShaderType::Vertex)->SetParam("worldMatrix", &worldMatrix, 16);
	mvpMatrix.transpose();
	rabbitShader->Get(EShaderType::Vertex)->SetParam("wvpMatrix", &mvpMatrix, 16);
}